#include "event_handler_game.h"

#include "camera.h"
#include "vecUtils.h"

#define ROTATION_ANGLE_PS 60.f // PS = per second
#define ROTATION_ANGLE_MOUSE 0.1f
#define TIME_TRANSFER_MS 100
#define MIN_DIST_TO_DEFINE_DRAG 40

#define MAX_GROUND_ANGLE_FOR_CAM_POV (-10.f)
#define GROUND_ANGLE_TOLERANCE_GOD 15.f

EventHandlerGame::EventHandlerGame(GameGame& game) :
  EventHandler::EventHandler(),
  _maxScalarProductWithGroundPOV(-sin(RAD*MAX_GROUND_ANGLE_FOR_CAM_POV)),
  _minScalarProductWithGroundGod(-sin(RAD*GROUND_ANGLE_TOLERANCE_GOD)),
  _draggingCamera(false),
  _game(game) {
  _game.setPovCamera(false);
}

void EventHandlerGame::handleKeyPressed(const sf::Event& event) {
  Camera& cam = Camera::getInstance();

  glm::vec2 moveFocused = _game.getFocusedPos();
  float theta = cam.getTheta()*M_PI/180.f;

  switch(event.key.code) {
    case sf::Keyboard::Num1:
      if (_game.getPovCamera())
        resetCamera(false);
      break;

    case sf::Keyboard::Num2:
      if (!_game.getPovCamera())
        resetCamera(true);
      break;

    case sf::Keyboard::S:
      moveFocused += glm::vec2(cos(theta), sin(theta));
      break;

    case sf::Keyboard::D:
      moveFocused += glm::vec2(cos(theta+M_PI/2.f), sin(theta+M_PI/2.f));
      break;

    case sf::Keyboard::Z:
      moveFocused += glm::vec2(cos(theta+M_PI), sin(theta+M_PI));
      break;

    case sf::Keyboard::Q:
      moveFocused += glm::vec2(cos(theta-M_PI/2.f), sin(theta-M_PI/2.f));
      break;

  }

  // Switch selection to closest character in the direction given by moveFocused
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) &&
      moveFocused != _game.getFocusedPos()) {

    _previousFocusedPos = _game.getFocusedPos();
    _game.changeFocusInDirection(moveFocused - _game.getFocusedPos());
    _transferStart.restart();
  }
}

void EventHandlerGame::handleKeyReleased(const sf::Event& event) {
  switch(event.key.code) {
    case sf::Keyboard::Z:
    case sf::Keyboard::Q:
    case sf::Keyboard::S:
    case sf::Keyboard::D:
    // The user is not moving the character any more
      if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Z) &&
          !sf::Keyboard::isKeyPressed(sf::Keyboard::Q) &&
          !sf::Keyboard::isKeyPressed(sf::Keyboard::S) &&
          !sf::Keyboard::isKeyPressed(sf::Keyboard::D) &&
          !sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
        _game.stopMoving();
      break;
  }
}

bool EventHandlerGame::handleEvent(const sf::Event& event, EventHandlerType& currentHandler) {
  Camera& cam = Camera::getInstance();

  if (event.type == sf::Event::MouseButtonPressed) {
    _oldPhi = cam.getPhi();
    _oldTheta = cam.getTheta();
  }

  else if (event.type == sf::Event::MouseButtonReleased) {
    if (!_draggingCamera) {
      glm::vec2 previousPos = _game.getFocusedPos();
      _game.moveCharacter(glm::ivec2(event.mouseButton.x, event.mouseButton.y));

      if (previousPos != _game.getFocusedPos()) {
        _transferStart.restart();
        _previousFocusedPos = previousPos;
      }
    }

    _draggingCamera = false;
  }

  else if (event.type == sf::Event::MouseMoved) {
    if (_beginDragLeft != glm::ivec2(0,0)) {
      if (_game.getPovCamera()) {
        _oldTheta = cam.getTheta();
        _oldPhi = cam.getPhi();
        cam.setTheta(_oldTheta + (event.mouseMove.x - _beginDragLeft.x) * ROTATION_ANGLE_MOUSE);
        cam.setPhi(_oldPhi + (event.mouseMove.y - _beginDragLeft.y) * ROTATION_ANGLE_MOUSE);
        _beginDragLeft.x = event.mouseMove.x;
        _beginDragLeft.y = event.mouseMove.y;
        _draggingCamera = true;
      }

      else {
        glm::ivec2 newMousePos = glm::ivec2(event.mouseMove.x,event.mouseMove.y);
        if (vu::norm(_beginDragLeft - newMousePos) > MIN_DIST_TO_DEFINE_DRAG)
          _draggingCamera = true;

        if (_draggingCamera) {
          cam.setTheta(_oldTheta);

          // Dragging the camera must be coherent when the mouse goes around the central character
          // The sense of rotation depends inwhich quarter of the screen the cursor is
          // If the quarter changes, we reset the origin of the dragging
          if (_beginDragLeft.x > cam.getW() / 2.f)
              cam.rotate( (event.mouseMove.y - _beginDragLeft.y) * ROTATION_ANGLE_MOUSE, 0);
          else
              cam.rotate(-(event.mouseMove.y - _beginDragLeft.y) * ROTATION_ANGLE_MOUSE, 0);

          if (_beginDragLeft.y < cam.getH() / 2.f)
              cam.rotate( (event.mouseMove.x - _beginDragLeft.x) * ROTATION_ANGLE_MOUSE, 0);
          else
              cam.rotate(-(event.mouseMove.x - _beginDragLeft.x) * ROTATION_ANGLE_MOUSE, 0);

          _beginDragLeft = newMousePos;
          _oldTheta = cam.getTheta();
        }
      }
    }
  }

  else if (event.type == sf::Event::KeyPressed)
    handleKeyPressed(event);

  else if (event.type == sf::Event::KeyReleased)
    handleKeyReleased(event);

  return EventHandler::handleEvent(event, currentHandler);
}

void EventHandlerGame::handleCamBoundsGodMode(float& theta) const {
  glm::vec3 normal = _game.getEngine().getNormalOnCameraPointedPos();

  // The steeper the slope, the less the user can move around
  float actualMinScalarProduct = 1 - (1 - _minScalarProductWithGroundGod) * (1-sin(acos(normal.z)));

  // We make the camera move only if the new theta is in the allowed zone
  if (vu::dot(normal, vu::carthesian(1, theta, 90)) < actualMinScalarProduct) {
    // New theta with given phi
    std::pair<float,float> thetasLim = solveAcosXplusBsinXequalC(
      normal.x, normal.y, actualMinScalarProduct);

    std::pair<float,float> distsToThetasLim;
    distsToThetasLim.first  = absDistBetweenAngles(theta, thetasLim.first);
    distsToThetasLim.second = absDistBetweenAngles(theta, thetasLim.second);

    if (distsToThetasLim.first < distsToThetasLim.second)
      theta = thetasLim.first;
    else
      theta = thetasLim.second;
  }
}

void EventHandlerGame::handleCamBoundsPOVMode(float& theta, float& phi) const {
  glm::vec3 normal = _game.getEngine().getNormalOnCameraPointedPos();

  // We make the camera move only if the new pointed direction is not too close to the ground in angle
  if (vu::dot(normal, vu::carthesian(1, theta, phi)) > _maxScalarProductWithGroundPOV) {

    float nPhi = getPhiLimForGivenTheta(theta, normal, _maxScalarProductWithGroundPOV);
    float phiIsTooFarByAmount = nPhi - phi;

    if (phiIsTooFarByAmount > 0) {
      float normalTheta = vu::spherical(normal).y;

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

void EventHandlerGame::onGoingEvents(const sf::Time& elapsed) {
  Camera& cam = Camera::getInstance();

  float transferProgress = _transferStart.getElapsedTime().asMilliseconds() / (float) TIME_TRANSFER_MS;

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

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        theta += ROTATION_ANGLE_PS * elapsed.asSeconds();

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        theta -= ROTATION_ANGLE_PS * elapsed.asSeconds();

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        phi += ROTATION_ANGLE_PS * elapsed.asSeconds();

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        phi -= ROTATION_ANGLE_PS * elapsed.asSeconds();
    }

    handleCamBoundsPOVMode(theta, phi);
  }

  else {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
      theta += ROTATION_ANGLE_PS * elapsed.asSeconds();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
      theta -= ROTATION_ANGLE_PS * elapsed.asSeconds();

    handleCamBoundsGodMode(theta);
  }

  cam.setValues(cam.getZoom(), theta, phi);

  if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
    glm::vec2 moveFocused = _game.getFocusedPos();
    float theta = cam.getTheta()*M_PI/180.f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
      moveFocused += glm::vec2(cos(theta), sin(theta));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
      moveFocused += glm::vec2(cos(theta+M_PI/2.f), sin(theta+M_PI/2.f));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
      moveFocused += glm::vec2(cos(theta+M_PI), sin(theta+M_PI));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
      moveFocused += glm::vec2(cos(theta-M_PI/2.f), sin(theta-M_PI/2.f));

    if (moveFocused != _game.getFocusedPos())
      _game.setTarget(moveFocused);
  }
}

void EventHandlerGame::resetCamera(bool pov) {
  Camera& cam = Camera::getInstance();

  _game.setPovCamera(pov);

  glm::vec3 terrainNormal = vu::spherical(_game.getEngine().getNormalOnCameraPointedPos());

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
  if (!_game.genTribe())
    return false;

  resetCamera(false);

  return true;
}

std::pair<float, float> EventHandlerGame::solveAcosXplusBsinXequalC(float a, float b, float c) {
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

float EventHandlerGame::absDistBetweenAngles(float a, float b) {
  return std::min(std::abs(a-b),std::abs(std::abs(a-b)-360));
}
