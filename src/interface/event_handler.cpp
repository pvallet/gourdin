#include "event_handler.h"
#include "camera.h"

bool EventHandler::handleEvent(const sf::Event& event, EventHandlerType& currentHandler) {
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

  else if (event.type == sf::Event::MouseButtonPressed) {
    if (event.mouseButton.button == sf::Mouse::Left)
      _beginDragLeft = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
  }

  else if (event.type == sf::Event::MouseButtonReleased) {
    if (event.mouseButton.button == sf::Mouse::Left)
      _beginDragLeft = sf::Vector2i(0,0);
  }

  return running;
}
