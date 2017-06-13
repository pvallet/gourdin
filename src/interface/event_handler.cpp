#include "event_handler.h"
#include "camera.h"
#include "clock.h"

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

      case sf::Keyboard::P:
        if (_paused)
          Clock::resumeGlobalTimer();
        else
          Clock::pauseGlobalTimer();
        switchPause();
        _paused = !_paused;
        break;
    }
  }

  else if (event.type == sf::Event::MouseButtonPressed) {
    if (event.mouseButton.button == sf::Mouse::Left)
      _beginDragLeft = glm::ivec2(event.mouseButton.x, event.mouseButton.y);
  }

  else if (event.type == sf::Event::MouseButtonReleased) {
    if (event.mouseButton.button == sf::Mouse::Left)
      _beginDragLeft = glm::ivec2(0,0);
  }

  return running;
}
