#include "event_handler_game.h"
#include "camera.h"

#define ROTATION_ANGLE_PS 60.f // PS = per second

EventHandlerGame::EventHandlerGame(Game& game, Interface& interface) :
  EventHandler::EventHandler(game, interface) {}

void EventHandlerGame::moveCamera(sf::Time elapsed) const {
  Camera& cam = Camera::getInstance();

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    cam.rotate(ROTATION_ANGLE_PS * elapsed.asSeconds(), 0.f);

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    cam.rotate(- ROTATION_ANGLE_PS * elapsed.asSeconds(), 0.f);
}

void EventHandlerGame::gainFocus() {
  Camera& cam = Camera::getInstance();
  cam.setZoom(MIN_R);

  if (_game.getTribe().size() == 0)
    _game.genTribe(cam.getPointedPos());
}
