#include "eventHandler.h"
#include "camera.h"
#include "clock.h"
#include "utils.h"

Uint32 EventHandler::SDL_USER_CLICK = SDL_RegisterEvents(1);
Uint32 EventHandler::SDL_USER_DOUBLE_CLICK = SDL_RegisterEvents(1);
Uint32 EventHandler::SDL_USER_LONG_CLICK_BEGIN = SDL_RegisterEvents(1);
Uint32 EventHandler::SDL_USER_LONG_CLICK_MOTION = SDL_RegisterEvents(1);
Uint32 EventHandler::SDL_USER_LONG_CLICK_END = SDL_RegisterEvents(1);
Uint32 EventHandler::SDL_USER_DRAG_BEGIN = SDL_RegisterEvents(1);
Uint32 EventHandler::SDL_USER_DRAG_MOTION = SDL_RegisterEvents(1);
Uint32 EventHandler::SDL_USER_DRAG_END = SDL_RegisterEvents(1);
bool EventHandler::_duringLongClick = false;
bool EventHandler::_duringDrag = false;
size_t EventHandler::_nbFingers = 0;

EventHandler::EventHandler(Game& game):
  _game(game),
  _beginDrag(DEFAULT_OUTSIDE_WINDOW_COORD),
  _pendingClick(DEFAULT_OUTSIDE_WINDOW_COORD),
  _gonnaBeDoubleClick(false) {

  SDL_SetEventFilter(EventHandler::HandleAppEvents, NULL);
}

bool EventHandler::isCloseEnoughToBeginClickToDefineClick(glm::ivec2 pos) const {
  glm::vec2 beginDrag(_beginDrag);
  glm::vec2 posFloat(pos);

  return glm::length(beginDrag - posFloat) < MAX_DIST_FOR_CLICK;
}

void EventHandler::sendEvent(Uint32 type, glm::ivec2 pos) const {
  SDL_Event event;
  SDL_zero(event);
  event.type = type;

  event.user.data1 = (void*) ((intptr_t) pos.x);
  event.user.data2 = (void*) ((intptr_t) pos.y);
  SDL_PushEvent(&event);
}

bool EventHandler::handleEvent(const SDL_Event& event) {
  bool running = true;

  switch (event.type) {
    case SDL_QUIT:
      running = false;
      break;

#ifndef __ANDROID__
    case SDL_WINDOWEVENT:
      if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
        Camera& cam = Camera::getInstance();
        cam.resize(event.window.data1, event.window.data2);
      }
      break;
#endif

    case SDL_MOUSEBUTTONDOWN:
      if (event.button.which == SDL_TOUCH_MOUSEID || event.button.button == SDL_BUTTON_LEFT)
        _nbFingers++;

      if (getNbFingers() == 1) {
        _beginDrag.x = event.button.x;
        _beginDrag.y = event.button.y;

        if (_pendingClick != DEFAULT_OUTSIDE_WINDOW_COORD) {

          if (_clickBegin.getElapsedTime() < DOUBLECLICK_MS &&
            isCloseEnoughToBeginClickToDefineClick(_pendingClick)) {

            _gonnaBeDoubleClick = true;
          }
        }

        _clickBegin.restart();
      }
      break;

    case SDL_MOUSEMOTION:
      _currentCursorPos.x = event.button.x;
      _currentCursorPos.y = event.button.y;

      if (getNbFingers() == 1) {
        if (isDuringLongClick())
          sendEvent(SDL_USER_LONG_CLICK_MOTION, _currentCursorPos);

        else if (!isCloseEnoughToBeginClickToDefineClick(_currentCursorPos) && !isDraggingCursor()) {
          sendEvent(SDL_USER_DRAG_BEGIN, _currentCursorPos);
          _duringDrag = true;
        }

        else if (isDraggingCursor())
          sendEvent(SDL_USER_DRAG_MOTION, _currentCursorPos);
      }
      break;

    case SDL_MOUSEBUTTONUP: {
      if (event.button.which == SDL_TOUCH_MOUSEID || event.button.button == SDL_BUTTON_LEFT)
        _nbFingers--;

      glm::ivec2 releasedPos(event.button.x, event.button.y);

      if (getNbFingers() == 0) {
        if (isCloseEnoughToBeginClickToDefineClick(releasedPos)) {

          if (_clickBegin.getElapsedTime() < LONGCLICK_MS) {

            if (_gonnaBeDoubleClick) {
              sendEvent(SDL_USER_DOUBLE_CLICK, _pendingClick);
              _pendingClick = DEFAULT_OUTSIDE_WINDOW_COORD;
            }

            else {
              sendEvent(SDL_USER_CLICK, _beginDrag);
              _pendingClick = _beginDrag;
            }
          }
        }

        if (isDuringLongClick()) {
          sendEvent(SDL_USER_LONG_CLICK_END, releasedPos);

          _pendingClick = DEFAULT_OUTSIDE_WINDOW_COORD;
          _duringLongClick = false;
        }

        else if (isDraggingCursor()) {
          sendEvent(SDL_USER_DRAG_END, releasedPos);
          _duringDrag = false;
        }

        _beginDrag = DEFAULT_OUTSIDE_WINDOW_COORD;
      }

      _gonnaBeDoubleClick = false;
    }
    break;

    case SDL_KEYDOWN:
      switch(event.key.keysym.sym) {
        case SDLK_ESCAPE:
          running = false;
          break;

        case SDLK_l:
          _game.switchLog();
          break;

        case SDLK_p:
          if (Clock::isGlobalTimerPaused())
            Clock::resumeGlobalTimer();
          else
            Clock::pauseGlobalTimer();
          break;
      }
      break;
  }

  // userEventLog(event);

  return running;
}

void EventHandler::onGoingEvents(int msElapsed) {
  if (!isDuringLongClick() && getNbFingers() == 1) {
    if (_clickBegin.getElapsedTime() > LONGCLICK_MS && !isDraggingCursor()) {

      sendEvent(SDL_USER_LONG_CLICK_BEGIN, _currentCursorPos);
      _duringLongClick = true;
    }
  }

  (void) msElapsed;
}

std::pair<float, float> EventHandler::solveAcosXplusBsinXequalC(float a, float b, float c) {
  // The code is a transcription of Wolfram Alpha solution
  std::pair<float,float> res(0,0);

  if (a == -c) {
    res.first = 180;
    res.second = 180;
  }

  else {
    float underRoot = a*a + b*b - c*c;
    if (underRoot >= 0) {
      res.first  = 2 * atan((b - sqrt(underRoot)) / (a + c)) / RAD;
      res.second = 2 * atan((b + sqrt(underRoot)) / (a + c)) / RAD;

      // Results are in range (-180, 180], make them in range [0,360)
      if (res.first < 0)
        res.first += 360;
      if (res.second < 0)
        res.second += 360;
    }

    // Ensure the ascending order
    if (res.second < res.first)
      std::swap(res.first,res.second);

  }
  return res;
}

float EventHandler::absDistBetweenAngles(float a, float b) {
  return std::min(std::abs(a-b),std::abs(std::abs(a-b)-360));
}

void EventHandler::makeThetaFitInAllowedZone(float& theta, const glm::vec3& normal, float minDotProduct) {
  if (glm::dot(normal, ut::carthesian(1, theta, 90)) < minDotProduct) {
    // New theta with given phi
    std::pair<float,float> thetasLim = solveAcosXplusBsinXequalC(
      normal.x, normal.y, minDotProduct);

    std::pair<float,float> distsToThetasLim;
    distsToThetasLim.first  = absDistBetweenAngles(theta, thetasLim.first);
    distsToThetasLim.second = absDistBetweenAngles(theta, thetasLim.second);

    if (distsToThetasLim.first < distsToThetasLim.second)
      theta = thetasLim.first;
    else
      theta = thetasLim.second;
  }
}

void EventHandler::userEventLog(const SDL_Event& event) const {
  if (event.type == SDL_USER_CLICK)
    SDL_Log("Click: (%d,%d)", (int) ((intptr_t) event.user.data1), (int) ((intptr_t) event.user.data2));
  if (event.type == SDL_USER_DOUBLE_CLICK)
    SDL_Log("Double click: (%d,%d)", (int) ((intptr_t) event.user.data1), (int) ((intptr_t) event.user.data2));
  if (event.type == SDL_USER_LONG_CLICK_BEGIN)
    SDL_Log("Long click begin: (%d,%d)", (int) ((intptr_t) event.user.data1), (int) ((intptr_t) event.user.data2));
  if (event.type == SDL_USER_LONG_CLICK_MOTION)
    SDL_Log("Long click motion: (%d,%d)", (int) ((intptr_t) event.user.data1), (int) ((intptr_t) event.user.data2));
  if (event.type == SDL_USER_LONG_CLICK_END)
    SDL_Log("Long click end: (%d,%d)", (int) ((intptr_t) event.user.data1), (int) ((intptr_t) event.user.data2));
  if (event.type == SDL_USER_DRAG_BEGIN)
    SDL_Log("Drag begin: (%d,%d)", (int) ((intptr_t) event.user.data1), (int) ((intptr_t) event.user.data2));
  if (event.type == SDL_USER_DRAG_MOTION)
    SDL_Log("Drag motion: (%d,%d)", (int) ((intptr_t) event.user.data1), (int) ((intptr_t) event.user.data2));
  if (event.type == SDL_USER_DRAG_END)
    SDL_Log("Drag end: (%d,%d)", (int) ((intptr_t) event.user.data1), (int) ((intptr_t) event.user.data2));
}

int EventHandler::HandleAppEvents(void *userdata, SDL_Event *event) {
  (void) userdata;

  switch (event->type) {
    case SDL_APP_DIDENTERFOREGROUND:
      Clock::resumeGlobalTimer();
      return 0;

    case SDL_APP_WILLENTERBACKGROUND:
      Clock::pauseGlobalTimer();
      return 0;

    default:
        return 1;
  }
}
