#include "event_handler_sandbox.h"
#include "camera.h"

#define ROTATION_ANGLE_PMS 0.06f // PMS = per millisecond
#define TRANSLATION_VALUE_PMS 0.0007f
#define SCROLL_SPEED_SLOW 30.f
#define SCROLL_SPEED_FAST 250.f

EventHandlerSandbox::EventHandlerSandbox(GameSandbox& game) :
  EventHandler::EventHandler(game),
  _addSelect(false),
  _scrollSpeed(SCROLL_SPEED_SLOW),
  _game(game) {
  _game.setScrollSpeedToSlow(true);
}

void EventHandlerSandbox::handleClick(const SDL_Event& event) {
  Camera& cam = Camera::getInstance();
  glm::vec2 minimapCoord = _game.getInterface().getMinimapClickCoords(event.button.x, event.button.y);

  if (minimapCoord.x >= 0 && minimapCoord.x <= 1 && minimapCoord.y >= 0 && minimapCoord.y <= 1) {
    cam.setPointedPos(MAX_COORD * minimapCoord);
  }

  else {
    const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);

    // Begin selection
    if (event.button.button == SDL_BUTTON_LEFT) {
      if (keyboardState[SDL_SCANCODE_LSHIFT])
          _addSelect = true;
      else
          _addSelect = false;

      _rectSelect = glm::ivec4(event.button.x, event.button.y,0,0);
      _game.getInterface().setRectSelect(_rectSelect);
    }

    // Move selection
    if (event.button.button == SDL_BUTTON_RIGHT) {
      if (_game.isSelectionEmpty())
        _game.createLion(glm::ivec2(event.button.x, event.button.y));
      else {
        _game.moveSelection(glm::ivec2(event.button.x, event.button.y));
        if (keyboardState[SDL_SCANCODE_LSHIFT] ||
            keyboardState[SDL_SCANCODE_RSHIFT])
          _game.makeLionsRun();
      }
    }
  }
}

void EventHandlerSandbox::handleKeyPressed(const SDL_Event& event) {
  switch(event.key.keysym.scancode) {
    case SDL_SCANCODE_SPACE:
      _game.goBackToSelection();
      break;

    case SDL_SCANCODE_LSHIFT:
    case SDL_SCANCODE_RSHIFT:
      _game.switchLionsRun();
      break;

    // Delete first selected lion
    case SDL_SCANCODE_DELETE:
      _game.killLion();
      break;

    case SDL_SCANCODE_Q:
    case SDL_SCANCODE_RETURN:
      _game.selectAllLions();
      break;

    case SDL_SCANCODE_B:
      _game.benchmark();
      break;

    case SDL_SCANCODE_H:
      if (_game.huntHasStarted())
        _game.interruptHunt();
      else
        _game.startNewHunt();
      break;

    case SDL_SCANCODE_E:
      if (_game.getScrollSpeedSlow()) {
        _scrollSpeed = SCROLL_SPEED_FAST;
        _game.setScrollSpeedToSlow(false);
      }
      else {
        _scrollSpeed = SCROLL_SPEED_SLOW;
        _game.setScrollSpeedToSlow(true);
      }
      break;

    case SDL_SCANCODE_Z:
      _game.switchWireframe();
      break;
  }
}

bool EventHandlerSandbox::handleEvent(const SDL_Event& event, EventHandlerType& currentHandler) {
  Camera& cam = Camera::getInstance();

  switch (event.type) {
    case SDL_MOUSEWHEEL:
      cam.zoom(- _scrollSpeed * event.wheel.y);
      break;

    case SDL_MOUSEBUTTONDOWN:
      handleClick(event);
      break;

    case SDL_MOUSEBUTTONUP:
      if (event.button.button == SDL_BUTTON_LEFT) {
        _game.select(_rectSelect, _addSelect);
        _rectSelect = glm::ivec4(event.button.x, event.button.y,0,0);
        _game.getInterface().setRectSelect(_rectSelect);
      }
      break;

    case SDL_MOUSEMOTION:
      if (_beginDragLeft != glm::ivec2(-1,-1)) {
        _rectSelect.z = event.motion.x - _rectSelect.x;
        _rectSelect.w = event.motion.y - _rectSelect.y;
        _game.getInterface().setRectSelect(_rectSelect);
      }
      break;

    case SDL_KEYDOWN:
      if (!Clock::isGlobalTimerPaused())
        handleKeyPressed(event);
      break;
  }

  return EventHandler::handleEvent(event, currentHandler);
}

void EventHandlerSandbox::onGoingEvents(int msElapsed) {
  Camera& cam = Camera::getInstance();
  const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
  int mousePosX, mousePosY;
  SDL_GetMouseState(&mousePosX, &mousePosY);

  float realTranslationValue = TRANSLATION_VALUE_PMS * msElapsed * cam.getZoom();
  if (keyboardState[SDL_SCANCODE_UP] ||
      keyboardState[SDL_SCANCODE_W])
    cam.translate(0.f, - realTranslationValue);

  if (keyboardState[SDL_SCANCODE_DOWN] ||
      keyboardState[SDL_SCANCODE_S])
    cam.translate(0.f, realTranslationValue);

  if (mousePosX == 0)
    cam.translate(- realTranslationValue, 0.f);

  if (mousePosY == 0)
    cam.translate(0.f, - realTranslationValue);

  if (mousePosX == (int) cam.getWindowW() - 1)
    cam.translate(realTranslationValue, 0.f);

  if (mousePosY == (int) cam.getWindowH() - 1)
    cam.translate(0.f, realTranslationValue);

  float theta = cam.getTheta();
  if (keyboardState[SDL_SCANCODE_RIGHT] ||
      keyboardState[SDL_SCANCODE_D])
    theta += ROTATION_ANGLE_PMS * msElapsed;

  if (keyboardState[SDL_SCANCODE_LEFT] ||
      keyboardState[SDL_SCANCODE_A])
    theta -= ROTATION_ANGLE_PMS * msElapsed;

  cam.setTheta(theta);
}

bool EventHandlerSandbox::gainFocus() {
  Camera& cam = Camera::getInstance();
  cam.setValues(INIT_R, INIT_THETA, INIT_PHI);

  return true;
}
