#include "event_handler_game.h"

#include "camera.h"
#include "vecUtils.h"

#define ROTATION_ANGLE_PS 60.f // PS = per second

EventHandlerGame::EventHandlerGame(Game& game, Interface& interface) :
  EventHandler::EventHandler(game, interface),
  _povCamera(false) {}

void EventHandlerGame::handleKeyPressed(sf::Event event) {
  Camera& cam = Camera::getInstance();

  sf::Vector2f moveFocused = _focusedCharacter->getPos();
  float theta = cam.getTheta()*M_PI/180.f;

  switch(event.key.code) {
    case sf::Keyboard::Num1:
      if (_povCamera) {
        cam.setValues(MIN_R, cam.getTheta() + 180, INIT_PHI);
        cam.setAdditionalHeight(0);
      }
      _povCamera = false;
      break;

    case sf::Keyboard::Num2:
      if (!_povCamera) {
        cam.setValues(0.1, cam.getTheta() + 180, 90.f);
        cam.setAdditionalHeight(_focusedCharacter->getSize().y);
      }
      _povCamera = true;
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

    std::vector<Human*> tribe = _game.getTribe();
    sf::Vector2f direction = moveFocused - _focusedCharacter->getPos();
    float threshold = sqrt(2)/2.f;

    Human* closestHuman = _focusedCharacter;
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

    _focusedCharacter = closestHuman;
  }
}

void EventHandlerGame::handleKeyReleased(sf::Event event) {
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

bool EventHandlerGame::handleEvent(sf::Event event, EventHandlerType& currentHandler) {
  if (event.type == sf::Event::MouseButtonPressed)
    _focusedCharacter = _game.moveCharacter(
      sf::Vector2i(event.mouseButton.x, event.mouseButton.y), _focusedCharacter);

  else if (event.type == sf::Event::KeyPressed)
    handleKeyPressed(event);

  else if (event.type == sf::Event::KeyReleased)
    handleKeyReleased(event);

  return EventHandler::handleEvent(event, currentHandler);
}

void EventHandlerGame::onGoingEvents(sf::Time elapsed) {
  Camera& cam = Camera::getInstance();

  cam.setPointedPos(_focusedCharacter->getPos());

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
    cam.rotate(ROTATION_ANGLE_PS * elapsed.asSeconds(), 0.f);

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    cam.rotate(- ROTATION_ANGLE_PS * elapsed.asSeconds(), 0.f);

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

  if (_game.getTribe().size() == 0) {
    _game.genTribe(cam.getPointedPos());
    _focusedCharacter = _game.getTribe().front();
  }
}
