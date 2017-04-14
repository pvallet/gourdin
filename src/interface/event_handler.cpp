#include "event_handler.h"
#include "camera.h"

EventHandler::EventHandler(Game& game, Interface& interface):
  _game(game),
  _interface(interface) {}

bool EventHandler::handleEvent(sf::Event event, EventHandlerType& currentHandler) {
  bool running = true;

  if (event.type == sf::Event::Closed)
    running = false;

  else if (event.type == sf::Event::Resized) {
    Camera& cam = Camera::getInstance();
    cam.resize(event.size.width, event.size.height);
  }

  else if (event.type == sf::Event::KeyPressed) {
    switch(event.key.code) {
      case sf::Keyboard::Escape:
        running = false;
        break;

      case sf::Keyboard::M:
        if (currentHandler == HDLR_GAME)
          currentHandler = HDLR_SANDBOX;
        else
          currentHandler = HDLR_GAME;
        break;
    }
  }

  return running;
}
