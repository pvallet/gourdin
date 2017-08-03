#include "event_handler_game.h"

#include "camera.h"

#define ROTATION_ANGLE_PMS 0.06f // PMS = per millisecond
#define ROTATION_ANGLE_MOUSE 0.1f
#define TIME_TRANSFER_MS 100
#define MIN_DIST_TO_DEFINE_DRAG 40

#define MAX_GROUND_ANGLE_FOR_CAM_POV (-10.f)
#define GROUND_ANGLE_TOLERANCE_GOD 15.f

EventHandlerGame::EventHandlerGame(GameGame& game) :
  EventHandler::EventHandler(game),
  _maxScalarProductWithGroundPOV(-sin(RAD*MAX_GROUND_ANGLE_FOR_CAM_POV)),
  _minScalarProductWithGroundGod(-sin(RAD*GROUND_ANGLE_TOLERANCE_GOD)),
  _draggingCamera(false),
  _game(game) {}

void EventHandlerGame::handleClick(glm::ivec2 windowCoords) {
  glm::vec2 previousPos = _game.getFocusedPos();
  _game.moveCharacter(windowCoords);

  if (previousPos != _game.getFocusedPos()) {
    _transferStart.restart();
    _previousFocusedPos = previousPos;
  }
}

void EventHandlerGame::handleKeyPressed(const SDL_Event& event) {
  Camera& cam = Camera::getInstance();

  glm::vec2 moveFocused = _game.getFocusedPos();
  float theta = cam.getTheta()*M_PI/180.f;

  switch(event.key.keysym.scancode) {
    case SDL_SCANCODE_1:
      if (_game.getPovCamera())
        resetCamera(false);
      break;

    case SDL_SCANCODE_2:
      if (!_game.getPovCamera())
        resetCamera(true);
      break;

    case SDL_SCANCODE_S:
      moveFocused += glm::vec2(cos(theta), sin(theta));
      break;

    case SDL_SCANCODE_D:
      moveFocused += glm::vec2(cos(theta+M_PI/2.f), sin(theta+M_PI/2.f));
      break;

    case SDL_SCANCODE_W:
      moveFocused += glm::vec2(cos(theta+M_PI), sin(theta+M_PI));
      break;

    case SDL_SCANCODE_A:
      moveFocused += glm::vec2(cos(theta-M_PI/2.f), sin(theta-M_PI/2.f));
      break;

  }

  // Switch selection to closest character in the direction given by moveFocused
  const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
  if (keyboardState[SDL_SCANCODE_LSHIFT] &&
      moveFocused != _game.getFocusedPos()) {

    _previousFocusedPos = _game.getFocusedPos();
    _game.changeFocusInDirection(moveFocused - _game.getFocusedPos());
    _transferStart.restart();
  }
}

void EventHandlerGame::handleKeyReleased(const SDL_Event& event) {
  switch(event.key.keysym.scancode) {
    case SDL_SCANCODE_Z:
    case SDL_SCANCODE_Q:
    case SDL_SCANCODE_S:
    case SDL_SCANCODE_D:
    // The user is not moving the character any more
    const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
      if (!keyboardState[SDL_SCANCODE_Z] &&
          !keyboardState[SDL_SCANCODE_Q] &&
          !keyboardState[SDL_SCANCODE_S] &&
          !keyboardState[SDL_SCANCODE_D] &&
          !keyboardState[SDL_SCANCODE_LSHIFT])
        _game.stopMoving();
      break;
  }
}

bool EventHandlerGame::handleEvent(const SDL_Event& event, EventHandlerType& currentHandler) {
  Camera& cam = Camera::getInstance();

  switch (event.type) {
    case SDL_MULTIGESTURE:
      currentHandler = HDLR_SANDBOX;
      break;

    case SDL_MOUSEBUTTONDOWN:
      _oldPhi = cam.getPhi();
      _oldTheta = cam.getTheta();
      break;

    case SDL_MOUSEBUTTONUP:
      if (!_draggingCamera && event.button.which != SDL_TOUCH_MOUSEID)
        handleClick(glm::ivec2(event.button.x, event.button.y));

      _draggingCamera = false;
      break;

    case SDL_MOUSEMOTION:
      if (_beginDragLeft != DEFAULT_OUTSIDE_WINDOW_COORD) {
        if (_game.getPovCamera()) {
          _oldTheta = cam.getTheta();
          _oldPhi = cam.getPhi();
          cam.setTheta(_oldTheta + (event.motion.x - _beginDragLeft.x) * ROTATION_ANGLE_MOUSE);
          cam.setPhi(_oldPhi + (event.motion.y - _beginDragLeft.y) * ROTATION_ANGLE_MOUSE);
          _beginDragLeft.x = event.motion.x;
          _beginDragLeft.y = event.motion.y;
          _draggingCamera = true;
        }

        else {
          glm::vec2 newMousePos(event.motion.x,event.motion.y);
          glm::vec2 beginDragLeft(_beginDragLeft);
          if (glm::length(beginDragLeft - newMousePos) > MIN_DIST_TO_DEFINE_DRAG)
            _draggingCamera = true;

          if (_draggingCamera) {
            cam.setTheta(_oldTheta);

            // Dragging the camera must be coherent when the mouse goes around the central character
            // The sense of rotation depends inwhich quarter of the screen the cursor is
            // If the quarter changes, we reset the origin of the dragging
            if (_beginDragLeft.x > cam.getW() / 2.f)
                cam.rotate( (event.motion.y - _beginDragLeft.y) * ROTATION_ANGLE_MOUSE, 0);
            else
                cam.rotate(-(event.motion.y - _beginDragLeft.y) * ROTATION_ANGLE_MOUSE, 0);

            if (_beginDragLeft.y < cam.getH() / 2.f)
                cam.rotate( (event.motion.x - _beginDragLeft.x) * ROTATION_ANGLE_MOUSE, 0);
            else
                cam.rotate(-(event.motion.x - _beginDragLeft.x) * ROTATION_ANGLE_MOUSE, 0);

            _beginDragLeft = newMousePos;
            _oldTheta = cam.getTheta();
          }
        }
      }
    break;

    case SDL_KEYDOWN:
      handleKeyPressed(event);
      break;

    case SDL_KEYUP:
      handleKeyReleased(event);
      break;
  }

  if (event.type == SDL_USER_FINGER_CLICK)
    handleClick(glm::ivec2((uintptr_t) event.user.data1, (uintptr_t) event.user.data2));

  else if (event.type == SDL_USER_FINGER_DOUBLE_CLICK) {
    if (_game.getPovCamera())
      resetCamera(false);
    else
      resetCamera(true);
  }

  return EventHandler::handleEvent(event, currentHandler);
}

void EventHandlerGame::handleCamBoundsGodMode(float& theta) const {
  glm::vec3 normal = _game.getEngine().getNormalOnCameraPointedPos();

  // The steeper the slope, the less the user can move around
  float actualMinScalarProduct = 1 - (1 - _minScalarProductWithGroundGod) * (1-sin(acos(normal.z)));

  makeThetaFitInAllowedZone(theta, normal, actualMinScalarProduct);
}

void EventHandlerGame::handleCamBoundsPOVMode(float& theta, float& phi) const {
  glm::vec3 normal = _game.getEngine().getNormalOnCameraPointedPos();

  // We make the camera move only if the new pointed direction is not too close to the ground in angle
  if (glm::dot(normal, ut::carthesian(1, theta, phi)) > _maxScalarProductWithGroundPOV) {

    float nPhi = getPhiLimForGivenTheta(theta, normal, _maxScalarProductWithGroundPOV);
    float phiIsTooFarByAmount = nPhi - phi;

    if (phiIsTooFarByAmount > 0) {
      float normalTheta = ut::spherical(normal).y;

      if (firstIsOnPositiveSideOfSecond(theta, normalTheta))
        theta += phiIsTooFarByAmount;
      else
        theta -= phiIsTooFarByAmount;

      if (firstIsOnPositiveSideOfSecond(theta, normalTheta) !=
          firstIsOnPositiveSideOfSecond(_oldTheta, normalTheta) &&
          absDistBetweenAngles(theta, normalTheta + 180) < 90 &&
          phi != _oldPhi)
        theta = normalTheta + 180;

      phi = getPhiLimForGivenTheta(theta, normal, _maxScalarProductWithGroundPOV);
    }
  }

  if (phi < 0)
    phi = 0;
  if (phi > 180)
    phi = 180;
}

void EventHandlerGame::onGoingEvents(int msElapsed) {
  EventHandler::onGoingEvents(msElapsed);

  Camera& cam = Camera::getInstance();
  const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);

  float transferProgress = _transferStart.getElapsedTime() / (float) TIME_TRANSFER_MS;

  if (transferProgress > 1)
    cam.setPointedPos(_game.getFocusedPos());
  else
    cam.setPointedPos(_previousFocusedPos + transferProgress *
      (_game.getFocusedPos() - _previousFocusedPos));

  float theta = cam.getTheta();
  float phi   = cam.getPhi();

  if (_game.getPovCamera()) {
    if (!_draggingCamera) {
      _oldPhi = phi; _oldTheta = theta;

      if (keyboardState[SDL_SCANCODE_LEFT])
        theta += ROTATION_ANGLE_PMS * msElapsed;

      if (keyboardState[SDL_SCANCODE_RIGHT])
        theta -= ROTATION_ANGLE_PMS * msElapsed;

      if (keyboardState[SDL_SCANCODE_UP])
        phi += ROTATION_ANGLE_PMS * msElapsed;

      if (keyboardState[SDL_SCANCODE_DOWN])
        phi -= ROTATION_ANGLE_PMS * msElapsed;
    }

    handleCamBoundsPOVMode(theta, phi);
  }

  else {
    if (keyboardState[SDL_SCANCODE_E])
      theta += ROTATION_ANGLE_PMS * msElapsed;

    if (keyboardState[SDL_SCANCODE_Q])
      theta -= ROTATION_ANGLE_PMS * msElapsed;

    handleCamBoundsGodMode(theta);
  }

  cam.setValues(cam.getZoom(), theta, phi);

  if (!keyboardState[SDL_SCANCODE_LSHIFT]) {
    glm::vec2 moveFocused = _game.getFocusedPos();
    float theta = cam.getTheta()*M_PI/180.f;

    if (keyboardState[SDL_SCANCODE_S])
      moveFocused += glm::vec2(cos(theta), sin(theta));
    if (keyboardState[SDL_SCANCODE_D])
      moveFocused += glm::vec2(cos(theta+M_PI/2.f), sin(theta+M_PI/2.f));
    if (keyboardState[SDL_SCANCODE_W])
      moveFocused += glm::vec2(cos(theta+M_PI), sin(theta+M_PI));
    if (keyboardState[SDL_SCANCODE_A])
      moveFocused += glm::vec2(cos(theta-M_PI/2.f), sin(theta-M_PI/2.f));

    if (moveFocused != _game.getFocusedPos())
      _game.setTarget(moveFocused);
  }
}

void EventHandlerGame::resetCamera(bool pov) {
  Camera& cam = Camera::getInstance();

  _game.setPovCamera(pov);

  glm::vec3 terrainNormal = ut::spherical(_game.getEngine().getNormalOnCameraPointedPos());

  if (pov) {
    cam.setValues(0.1, terrainNormal.y + 180, 90.f - cam.getFov() / 2.f);
    cam.setAdditionalHeight(_game.getCharacterHeight());
  }

  else {
    cam.setValues(MIN_R, terrainNormal.y, INIT_PHI);
    cam.setAdditionalHeight(0);
  }
}

bool EventHandlerGame::gainFocus() {
  _game.deleteTribe();

  if (!_game.genTribe())
    return false;

  resetCamera(false);

  return true;
}

float EventHandlerGame::getPhiLimForGivenTheta(float theta, glm::vec3 normal, float maxDotProduct) {
  return solveAcosXplusBsinXequalC(
    normal.z, cos(theta*RAD)*normal.x + sin(theta*RAD)*normal.y, maxDotProduct).first;
}

bool EventHandlerGame::firstIsOnPositiveSideOfSecond(float first, float second) {
  float secondAnglePlus90 = second + 90;
  if (secondAnglePlus90 > 360)
    secondAnglePlus90 -= 360;
  return absDistBetweenAngles(first, secondAnglePlus90) < 90;
}
