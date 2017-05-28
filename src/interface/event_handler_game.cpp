#include "event_handler_game.h"

#include "camera.h"
#include "vecUtils.h"

#define ROTATION_ANGLE_PS 60.f // PS = per second
#define ROTATION_ANGLE_MOUSE 0.1f
#define TIME_TRANSFER_MS 100
#define MIN_DIST_TO_DEFINE_DRAG 40

#define MIN_CAM_ANGLE_WITH_GROUND 30.f

EventHandlerGame::EventHandlerGame(Game& game, Interface& interface) :
  EventHandler::EventHandler(game, interface),
  _minScalarProductWithGround(sin(RAD*MIN_CAM_ANGLE_WITH_GROUND)),
  _povCamera(false),
  _draggingCamera(false) {}

void EventHandlerGame::handleKeyPressed(const sf::Event& event) {
  Camera& cam = Camera::getInstance();

  sf::Vector2f moveFocused = _focusedCharacter->getPos();
  float theta = cam.getTheta()*M_PI/180.f;

  switch(event.key.code) {
    case sf::Keyboard::Num1:
      if (_povCamera) {
        cam.setValues(MIN_R, cam.getTheta() + 180, INIT_PHI);
        cam.setAdditionalHeight(0);
        _interface.setPovCamera(false);
        _povCamera = false;
      }
      break;

    case sf::Keyboard::Num2:
      if (!_povCamera) {
        cam.setValues(0.1, cam.getTheta() + 180, 90.f);
        cam.setAdditionalHeight(_focusedCharacter->getSize().y);
        _interface.setPovCamera(true);
        _povCamera = true;
      }
      break;

    case sf::Keyboard::S:
      moveFocused += sf::Vector2f(cos(theta), sin(theta));
      break;

    case sf::Keyboard::D:
      moveFocused += sf::Vector2f(cos(theta+M_PI/2.f), sin(theta+M_PI/2.f));
      break;

    case sf::Keyboard::Z:
      moveFocused += sf::Vector2f(cos(theta+M_PI), sin(theta+M_PI));
      break;

    case sf::Keyboard::Q:
      moveFocused += sf::Vector2f(cos(theta-M_PI/2.f), sin(theta-M_PI/2.f));
      break;

  }

  // Switch selection to closest character in the direction given by moveFocused
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) &&
      moveFocused != _focusedCharacter->getPos()) {

    std::vector<Controllable*> tribe = _game.getTribe();
    sf::Vector2f direction = moveFocused - _focusedCharacter->getPos();
    float threshold = sqrt(2)/2.f;

    Controllable* closestHuman = _focusedCharacter;
    float closestDist = MAX_COORD;

    for (size_t i = 0; i < tribe.size(); i++) {
      sf::Vector2f toChar = tribe[i]->getPos() - _focusedCharacter->getPos();
      float distance = vu::norm(toChar);

      // Character is in the right direction, with +- M_PI/4 margin
      if (vu::dot(toChar, direction)/distance > threshold) {

        // Checks whether the character is visible on the screen
        sf::IntRect screenCoord = tribe[i]->getScreenCoord();
        if (screenCoord.top > (int) cam.getH())
          continue;
        if (screenCoord.left > (int) cam.getW())
          continue;
        if (screenCoord.top + screenCoord.height < 0)
          continue;
        if (screenCoord.left + screenCoord.width < 0)
          continue;


        if (distance < closestDist) {
          closestDist = distance;
          closestHuman = tribe[i];
        }
      }
    }

    _previousFocusedPos = _focusedCharacter->getPos();
    _transferStart.restart();
    _focusedCharacter = closestHuman;
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
        _focusedCharacter->stop();
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
      Controllable* previous = _focusedCharacter;
      _focusedCharacter = _game.moveCharacter(
        sf::Vector2i(event.mouseButton.x, event.mouseButton.y), _focusedCharacter);

      if (previous != _focusedCharacter) {
        _transferStart.restart();
        _previousFocusedPos = previous->getPos();
      }
    }

    _draggingCamera = false;
  }

  else if (event.type == sf::Event::MouseMoved) {
    if (_beginDragLeft != sf::Vector2i(0,0)) {
      if (_povCamera) {
        cam.setTheta(_oldTheta + (event.mouseMove.x - _beginDragLeft.x) * ROTATION_ANGLE_MOUSE);
        cam.setPhi(_oldPhi + (event.mouseMove.y - _beginDragLeft.y) * ROTATION_ANGLE_MOUSE);
        _draggingCamera = true;
      }

      else {
        sf::Vector2i newMousePos = sf::Vector2i(event.mouseMove.x,event.mouseMove.y);
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

void EventHandlerGame::handleCameraGodMode(const sf::Time& elapsed, float& theta, float& phi) const {
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
    theta += ROTATION_ANGLE_PS * elapsed.asSeconds();

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    theta -= ROTATION_ANGLE_PS * elapsed.asSeconds();

  sf::Vector3f normal = _game.getNormalOnCameraPointedPos();

  // We make the camera move only if the new position is not too close to the ground in angle
  // Otherwise we find the closest available values
  if (vu::dot(normal, vu::carthesian(1, theta, phi)) < _minScalarProductWithGround) {
    std::pair<float,float> thetasLim = solveAcosXplusBsinXequalC(
      sin(phi*RAD)*normal.x, sin(phi*RAD)*normal.y, _minScalarProductWithGround - cos(phi*RAD)*normal.z);

    std::pair<float,float> distsToThetasLim;
    distsToThetasLim.first  = std::min(std::abs(theta-thetasLim.first),
                                       std::abs(std::abs(theta-thetasLim.first)-360));
    distsToThetasLim.second = std::min(std::abs(theta-thetasLim.second),
                                       std::abs(std::abs(theta-thetasLim.second)-360));

    if (distsToThetasLim.first < distsToThetasLim.second)
      theta = thetasLim.first;
    else
      theta = thetasLim.second;
  }
}

void EventHandlerGame::handleCameraPOVMode(const sf::Time& elapsed, float& theta, float& phi) const {
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    theta += ROTATION_ANGLE_PS * elapsed.asSeconds();

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    theta -= ROTATION_ANGLE_PS * elapsed.asSeconds();

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    phi += ROTATION_ANGLE_PS * elapsed.asSeconds();

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    phi -= ROTATION_ANGLE_PS * elapsed.asSeconds();
}

void EventHandlerGame::onGoingEvents(const sf::Time& elapsed) {
  Camera& cam = Camera::getInstance();

  float transferProgress = _transferStart.getElapsedTime().asMilliseconds() / (float) TIME_TRANSFER_MS;

  if (transferProgress > 1)
    cam.setPointedPos(_focusedCharacter->getPos());
  else
    cam.setPointedPos(_previousFocusedPos + transferProgress *
      (_focusedCharacter->getPos() - _previousFocusedPos));

  float theta = cam.getTheta();
  float phi   = cam.getPhi();

  if (_povCamera)
    handleCameraPOVMode(elapsed, theta, phi);
  else
    handleCameraGodMode(elapsed, theta, phi);

  cam.setValues(cam.getZoom(), theta, phi);

  if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
    sf::Vector2f moveFocused = _focusedCharacter->getPos();
    float theta = cam.getTheta()*M_PI/180.f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
      moveFocused += sf::Vector2f(cos(theta), sin(theta));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
      moveFocused += sf::Vector2f(cos(theta+M_PI/2.f), sin(theta+M_PI/2.f));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
      moveFocused += sf::Vector2f(cos(theta+M_PI), sin(theta+M_PI));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
      moveFocused += sf::Vector2f(cos(theta-M_PI/2.f), sin(theta-M_PI/2.f));

    if (moveFocused != _focusedCharacter->getPos())
      _focusedCharacter->setTarget(moveFocused);
  }
}

void EventHandlerGame::gainFocus() {
  Camera& cam = Camera::getInstance();
  cam.setZoom(MIN_R);

  _povCamera = false;
  _interface.setPovCamera(false);

  if (_game.getTribe().size() == 0) {
    _game.genTribe(cam.getPointedPos());
    _focusedCharacter = _game.getTribe().front();
  }
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
  }

  return res;
}
