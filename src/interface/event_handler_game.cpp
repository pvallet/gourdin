#include "event_handler_game.h"
#include "camera.h"

#define ROTATION_ANGLE_PS 60.f // PS = per second

EventHandlerGame::EventHandlerGame(Game& game, Interface& interface) :
  EventHandler::EventHandler(game, interface),
  _povCamera(false) {}

void EventHandlerGame::handleKeyPressed(sf::Event event) {
  Camera& cam = Camera::getInstance();

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
  }
}

bool EventHandlerGame::handleEvent(sf::Event event, EventHandlerType& currentHandler) {
  if (event.type == sf::Event::MouseButtonPressed)
    _focusedCharacter = _game.moveCharacter(
      sf::Vector2i(event.mouseButton.x, event.mouseButton.y), _focusedCharacter);

  else if (event.type == sf::Event::KeyPressed)
    handleKeyPressed(event);

  return EventHandler::handleEvent(event, currentHandler);
}

void EventHandlerGame::moveCamera(sf::Time elapsed) const {
  Camera& cam = Camera::getInstance();

  cam.setPointedPos(_focusedCharacter->getPos());

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    cam.rotate(ROTATION_ANGLE_PS * elapsed.asSeconds(), 0.f);

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    cam.rotate(- ROTATION_ANGLE_PS * elapsed.asSeconds(), 0.f);
}

void EventHandlerGame::gainFocus() {
  Camera& cam = Camera::getInstance();
  cam.setZoom(MIN_R);

  if (_game.getTribe().size() == 0) {
    _game.genTribe(cam.getPointedPos());
    _focusedCharacter = _game.getTribe().front();
  }
}
