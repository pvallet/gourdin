#include "controller.h"

#include <set>
#include <string>
#include <sstream>

#include "camera.h"
#include "clock.h"
#include "lion.h"
#include "utils.h"

Controller::Controller(sf::RenderWindow& window) :
  _running(true),
  _engine(),
  _gameGame(window,_engine),
  _gameSandbox(window,_engine),
  _eHandlerGame(_gameGame),
  _eHandlerSandbox(_gameSandbox),
  _currentHandlerType(HDLR_SANDBOX),
  _window(window) {

  Camera& cam = Camera::getInstance();
	cam.resize(window.getSize().x, window.getSize().y );
}

void Controller::init() {

#ifndef CORE_PROFILE
  _gameGame.init();
  _gameSandbox.init();
#endif

  _engine.init();
}

void Controller::run() {
  Clock frameClock;

  while (_running) {
    sf::Event event;
    _msElapsed = frameClock.restart();

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
          if (!_eHandlerGame.gainFocus()) {
            _currentHandlerType = HDLR_SANDBOX;
            _gameSandbox.displayError("Can't spawn a tribe here");
          }
        }
        else
          _eHandlerSandbox.gainFocus();
      }
    }

    if (_currentHandlerType == HDLR_GAME) {
      _eHandlerGame.onGoingEvents(_msElapsed);
      _gameGame.update(_msElapsed);
      _gameGame.render();
    }
    else {
      _eHandlerSandbox.onGoingEvents(_msElapsed);
      _gameSandbox.update(_msElapsed);
      _gameSandbox.render();
    }
  }
}
