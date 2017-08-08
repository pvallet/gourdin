#include "eventHandlerLockedView.h"

#include "camera.h"

#define ROTATION_ANGLE_PMS 0.06f // PMS = per millisecond
#define ROTATION_ANGLE_MOUSE 0.1f
#define TIME_TRANSFER_MS 100

#define MAX_GROUND_ANGLE_FOR_CAM_POV (-10.f)
#define GROUND_ANGLE_TOLERANCE_GOD 15.f

EventHandlerLockedView::EventHandlerLockedView(Game& game) :
  EventHandler::EventHandler(game),
  _maxScalarProductWithGroundPOV(-sin(RAD*MAX_GROUND_ANGLE_FOR_CAM_POV)),
  _minScalarProductWithGroundGod(-sin(RAD*GROUND_ANGLE_TOLERANCE_GOD)) {}

void EventHandlerLockedView::handleKeyPressed(const SDL_Event& event) {
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

void EventHandlerLockedView::handleKeyReleased(const SDL_Event& event) {
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

bool EventHandlerLockedView::handleEvent(const SDL_Event& event) {
  Camera& cam = Camera::getInstance();

  switch (event.type) {
    case SDL_MULTIGESTURE:
    case SDL_MOUSEWHEEL:
      _game.setLockedView(false);
      break;

    case SDL_KEYDOWN:
      handleKeyPressed(event);
      break;

    case SDL_KEYUP:
      handleKeyReleased(event);
      break;
  }

  if (event.type == SDL_USER_CLICK) {
    glm::ivec2 windowCoords((intptr_t) event.user.data1, (intptr_t) event.user.data2);

    glm::vec2 previousPos = _game.getFocusedPos();
    bool newSelectedCharacter = _game.pickCharacter(windowCoords);

    if (newSelectedCharacter) {
      _transferStart.restart();
      _previousFocusedPos = previousPos;
    }

    else
      _game.moveFocused(windowCoords);
  }

  else if (event.type == SDL_USER_DOUBLE_CLICK)
    _game.switchLionsRun();

  else if (event.type == SDL_USER_DRAG_BEGIN) {
    _oldPhi = cam.getPhi();
    _oldTheta = cam.getTheta();
    _beginDrag = getBeginDrag();
  }

  else if (event.type == SDL_USER_DRAG_MOTION) {
    if (_game.getPovCamera()) {
      cam.setTheta(_oldTheta + ((intptr_t) event.user.data1 - _beginDrag.x) * ROTATION_ANGLE_MOUSE);
      cam.setPhi  (_oldPhi   + ((intptr_t) event.user.data2 - _beginDrag.y) * ROTATION_ANGLE_MOUSE);
    }

    else {
      cam.setTheta(_oldTheta);

      // Dragging the camera must be coherent when the mouse goes around the central character
      // The sense of rotation depends on which quarter of the screen the cursor is
      // If the quarter changes, we reset the origin of the dragging
      if (_beginDrag.x > cam.getW() / 2.f)
          cam.rotate( ((intptr_t) event.user.data2 - _beginDrag.y) * ROTATION_ANGLE_MOUSE, 0);
      else
          cam.rotate(-((intptr_t) event.user.data2 - _beginDrag.y) * ROTATION_ANGLE_MOUSE, 0);

      if (_beginDrag.y < cam.getH() / 2.f)
          cam.rotate( ((intptr_t) event.user.data1 - _beginDrag.x) * ROTATION_ANGLE_MOUSE, 0);
      else
          cam.rotate(-((intptr_t) event.user.data1 - _beginDrag.x) * ROTATION_ANGLE_MOUSE, 0);

      _beginDrag.x = (intptr_t) event.user.data1;
      _beginDrag.y = (intptr_t) event.user.data2;
      _oldTheta = cam.getTheta();
    }
  }

  return EventHandler::handleEvent(event);
}

void EventHandlerLockedView::handleCamBoundsGodMode(float& theta) const {
  glm::vec3 normal = _game.getEngine().getNormalOnCameraPointedPos();

  // The steeper the slope, the less the user can move around
  float actualMinScalarProduct = 1 - (1 - _minScalarProductWithGroundGod) * (1-sin(acos(normal.z)));

  makeThetaFitInAllowedZone(theta, normal, actualMinScalarProduct);
}

void EventHandlerLockedView::handleCamBoundsPOVMode(float& theta, float& phi) const {
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

void EventHandlerLockedView::onGoingEvents(int msElapsed) {
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
    if (!EventHandler::isDraggingCursor()) {
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

void EventHandlerLockedView::resetCamera(bool pov) {
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

float EventHandlerLockedView::getPhiLimForGivenTheta(float theta, glm::vec3 normal, float maxDotProduct) {
  return solveAcosXplusBsinXequalC(
    normal.z, cos(theta*RAD)*normal.x + sin(theta*RAD)*normal.y, maxDotProduct).first;
}

bool EventHandlerLockedView::firstIsOnPositiveSideOfSecond(float first, float second) {
  float secondAnglePlus90 = second + 90;
  if (secondAnglePlus90 > 360)
    secondAnglePlus90 -= 360;
  return absDistBetweenAngles(first, secondAnglePlus90) < 90;
}
