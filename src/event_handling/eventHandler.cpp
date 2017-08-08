#include "eventHandler.h"
#include "camera.h"
#include "clock.h"
#include "utils.h"

Uint32 EventHandler::SDL_USER_CLICK = SDL_RegisterEvents(1);
Uint32 EventHandler::SDL_USER_LONG_CLICK_BEGIN = SDL_RegisterEvents(1);
Uint32 EventHandler::SDL_USER_LONG_CLICK_MOTION = SDL_RegisterEvents(1);
Uint32 EventHandler::SDL_USER_LONG_CLICK_END = SDL_RegisterEvents(1);
Uint32 EventHandler::SDL_USER_DOUBLE_CLICK = SDL_RegisterEvents(1);
bool EventHandler::_duringLongClick = false;
size_t EventHandler::_nbFingers = 0;

EventHandler::EventHandler(Game& game):
  _beginDrag(DEFAULT_OUTSIDE_WINDOW_COORD),
  _game(game),
  _pendingClick(DEFAULT_OUTSIDE_WINDOW_COORD),
  _gonnaBeDoubleClick(false) {

  SDL_SetEventFilter(EventHandler::HandleAppEvents, NULL);
}

bool EventHandler::isCloseEnoughToBeginClickToDefineClick(glm::ivec2 pos) const {
  glm::vec2 beginDragTouch(_beginDrag);
  glm::vec2 posFloat(pos);

  return glm::length(beginDragTouch - posFloat) < MAX_DIST_FOR_CLICK;
}

bool EventHandler::handleEvent(const SDL_Event& event) {
  bool running = true;

  switch (event.type) {
    case SDL_QUIT:
      running = false;
      break;

    case SDL_WINDOWEVENT:
      if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
        Camera& cam = Camera::getInstance();
        cam.resize(event.window.data1, event.window.data2);
      }
      break;

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
    if (getNbFingers() == 1) {

        _currentCursorPos.x = event.button.x;
        _currentCursorPos.y = event.button.y;

        // Send longClickEventMotion
        if (_duringLongClick) {
          SDL_Event longClickEventMotion;
          SDL_zero(longClickEventMotion);
          longClickEventMotion.type = SDL_USER_LONG_CLICK_MOTION;

          longClickEventMotion.user.data1 = (void*) ((uintptr_t) _currentCursorPos.x);
          longClickEventMotion.user.data2 = (void*) ((uintptr_t) _currentCursorPos.y);
          SDL_PushEvent(&longClickEventMotion);
        }
      }
      break;

    case SDL_MOUSEBUTTONUP: {
      if (event.button.which == SDL_TOUCH_MOUSEID || event.button.button == SDL_BUTTON_LEFT)
        _nbFingers--;

      glm::ivec2 releasedPos(event.button.x, event.button.y);

      if (getNbFingers() == 0) {
        if (isCloseEnoughToBeginClickToDefineClick(releasedPos)) {

          if (_clickBegin.getElapsedTime() < LONGCLICK_MS) {

            // Send doubleClickEvent
            if (_gonnaBeDoubleClick) {
              SDL_Event doubleClickEvent;
              SDL_zero(doubleClickEvent);
              doubleClickEvent.type = SDL_USER_DOUBLE_CLICK;

              doubleClickEvent.user.data1 = (void*) ((uintptr_t) _pendingClick.x);
              doubleClickEvent.user.data2 = (void*) ((uintptr_t) _pendingClick.y);
              SDL_PushEvent(&doubleClickEvent);

              _pendingClick = DEFAULT_OUTSIDE_WINDOW_COORD;
            }

            // Send clickEvent
            else {
              SDL_Event clickEvent;
              SDL_zero(clickEvent);
              clickEvent.type = SDL_USER_CLICK;

              clickEvent.user.data1 = (void*) ((uintptr_t) _beginDrag.x);
              clickEvent.user.data2 = (void*) ((uintptr_t) _beginDrag.y);
              SDL_PushEvent(&clickEvent);

              _pendingClick = _beginDrag;
            }
          }
        }

        // Send longClickEventEnd
        if (_duringLongClick) {
          SDL_Event longClickEventEnd;
          SDL_zero(longClickEventEnd);
          longClickEventEnd.type = SDL_USER_LONG_CLICK_END;

          longClickEventEnd.user.data1 = (void*) ((uintptr_t) releasedPos.x);
          longClickEventEnd.user.data2 = (void*) ((uintptr_t) releasedPos.y);
          SDL_PushEvent(&longClickEventEnd);

          _pendingClick = DEFAULT_OUTSIDE_WINDOW_COORD;
          _duringLongClick = false;
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

  return running;
}

void EventHandler::onGoingEvents(int msElapsed) {
  if (!_duringLongClick && getNbFingers() == 1) {
    if (_clickBegin.getElapsedTime() > LONGCLICK_MS &&
        isCloseEnoughToBeginClickToDefineClick(_currentCursorPos)) {

      SDL_Event longClickEventBegin;
      SDL_zero(longClickEventBegin);
      longClickEventBegin.type = SDL_USER_LONG_CLICK_BEGIN;

      longClickEventBegin.user.data1 = (void*) ((uintptr_t) _currentCursorPos.x);
      longClickEventBegin.user.data2 = (void*) ((uintptr_t) _currentCursorPos.y);
      SDL_PushEvent(&longClickEventBegin);

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
