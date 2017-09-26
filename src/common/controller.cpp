#include "controller.h"

#include <set>
#include <string>
#include <sstream>

#include "camera.h"
#include "clock.h"
#include "lion.h"

Controller::Controller(SDL2pp::Window& window) :
  _running(true),
  _loadingScreen(window),
  _engine(),
  _game(_engine),
  _eventHandlerLockedView(_game, window),
  _eventHandlerGlobalView(_game, window),
  _window(window) {

  Camera& cam = Camera::getInstance();
  cam.setWindowSize(window.GetWidth(), window.GetHeight());

#ifdef __ANDROID__
  // We lower the game resolution to increase performance. Interface is left untouched
  cam.resize(std::min(window.GetWidth(), 1280), std::min(window.GetHeight(), 720));
#else
  cam.resize(window.GetWidth(),window.GetHeight());
#endif

  cam.reset();
}

void Controller::init() {
  ColoredRectangles::loadShader();
  Text::loadShader();
  TexturedRectangle::loadShader();
  _engine.init(_loadingScreen);
  _game.init(_loadingScreen);
}


void Controller::run() {
  Clock frameClock;
  bool previousViewWasLocked = false;

  while (_running) {
    _msElapsed = frameClock.restart();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (_game.isViewLocked())
        _running = _eventHandlerLockedView.handleEvent(event);
      else
        _running = _eventHandlerGlobalView.handleEvent(event);

      if (!_running)
        break;
    }

    if (_game.isViewLocked() != previousViewWasLocked) {
      if (_game.isViewLocked())
        _eventHandlerLockedView.gainFocus();
      else
        _eventHandlerGlobalView.gainFocus();

      previousViewWasLocked = _game.isViewLocked();
    }

    if (_game.isViewLocked())
      _eventHandlerLockedView.onGoingEvents(_msElapsed);
    else
      _eventHandlerGlobalView.onGoingEvents(_msElapsed);

    _game.update(_msElapsed);
    _game.render();
    SDL_GL_SwapWindow(_window.Get());
  }
}
