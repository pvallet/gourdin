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
  _engine(),
  _gameGame(window,_engine,_interface),
  _gameSandbox(window,_engine,_interface),
  _eHandlerGame(_gameGame),
  _eHandlerSandbox(_gameSandbox),
  _currentHandlerType(HDLR_SANDBOX),
  _window(window) {

  Camera& cam = Camera::getInstance();
	cam.resize(window.getSize().x, window.getSize().y );
}

void Controller::init() {

#ifndef CORE_PROFILE
  _interface.init();
#endif

  _engine.init();
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
      else
        _running = _eHandlerSandbox.handleEvent(event,_currentHandlerType);

      if (!_running)
        break;

      if (_currentHandlerType != lastHandlerType) {
        if (_currentHandlerType == HDLR_GAME) {
          // If we can spawn a tribe we go back to sandbox mode
          if (!_eHandlerGame.gainFocus())
            _currentHandlerType = HDLR_SANDBOX;
        }
        else
          _eHandlerSandbox.gainFocus();
      }
    }

    if (_currentHandlerType == HDLR_GAME) {
      _eHandlerGame.onGoingEvents(_elapsed);
      _gameGame.update(_elapsed);
      _gameGame.render();
    }
    else {
      _eHandlerSandbox.onGoingEvents(_elapsed);
      _gameSandbox.update(_elapsed);
      _gameSandbox.render();
    }
  }
}
