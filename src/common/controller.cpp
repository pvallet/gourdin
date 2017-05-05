#include "controller.h"

#include <set>
#include <string>
#include <sstream>

#include "camera.h"
#include "lion.h"
#include "utils.h"

Controller::Controller(sf::RenderWindow& window) :
  _running(true),
  _interface(window),
  _game(),
  _eHandlerGame(_game,_interface),
  _eHandlerSandbox(_game,_interface),
  _currentHandlerType(HDLR_SANDBOX),
  _window(window) {

  Camera& cam = Camera::getInstance();
	cam.resize(window.getSize().x, window.getSize().y );
}

void Controller::init() {
  _interface.init();
  _game.init();
}

void Controller::render() const {
  if (_running) {
    _game.render();
    _window.pushGLStates();

    if (_currentHandlerType == HDLR_SANDBOX) {
      _game.renderLifeBars(_window);
      _interface.renderRectSelect();
      _interface.renderMinimap(_game);
    }

    _interface.renderInfo(_currentHandlerType == HDLR_GAME);

    if (_currentHandlerType == HDLR_SANDBOX)
      _interface.renderLog(_elapsed);

    _window.display();
    _window.popGLStates();
  }
}

void Controller::run() {
  sf::Clock frameClock;

  while (_running) {
    sf::Event event;
    _elapsed = frameClock.restart();

    while (_window.pollEvent(event)) {
      EventHandlerType lastHandlerType = _currentHandlerType;

      if (_currentHandlerType == HDLR_GAME)
        _running = _eHandlerGame.handleEvent(event,_currentHandlerType);
      else {
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::B)
          benchmark(100);
        _running = _eHandlerSandbox.handleEvent(event,_currentHandlerType);
      }

      if (_currentHandlerType != lastHandlerType) {
        if (_currentHandlerType == HDLR_GAME)
          _eHandlerGame.gainFocus();
        else
          _eHandlerSandbox.gainFocus();
      }
    }

    if (_currentHandlerType == HDLR_GAME)
      _eHandlerGame.onGoingEvents(_elapsed);
    else
      _eHandlerSandbox.onGoingEvents(_elapsed);

    _game.update(_elapsed);
    render();
  }
}

void Controller::benchmark(size_t range) {
  Camera& cam = Camera::getInstance();

  sf::Time totalElapsed, elapsed;
  sf::Clock frameClock;

  _game.resetCamera();

  for (size_t i = 0; i < range; i++) {
    elapsed = frameClock.restart();
    totalElapsed += elapsed;
    cam.zoom(5.f * i);
    _game.update(elapsed);
    render();
  }

  std::cout << "Rendered " << range << " frames in " << totalElapsed.asMilliseconds() << " milliseconds." << std::endl;
  std::cout << "Average FPS: " << 1.f / totalElapsed.asSeconds() * range << std::endl;

  _game.resetCamera();
}
