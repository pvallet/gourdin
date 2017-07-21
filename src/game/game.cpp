#include "game.h"
#include "camera.h"

#include <sstream>

Game::Game (Engine& engine):
  _engine(engine),
  _interface() {}

void Game::init() {
  _2DShader.load("src/shaders/2D.vert", "src/shaders/simpleTexture.frag");
  _interface.setEngineTexture(_engine.getColorBuffer());
  _interface.init();
  _interface.setTextTopLeft(getInfoText());
}

void Game::update(int msElapsed) {
  if (Clock::isGlobalTimerPaused())
    _interface.setTextCenter("PAUSED", 1);
  _engine.update(msElapsed);
}

std::string Game::getInfoText() const {
  std::ostringstream text;

  text << "Esc: " << "Quit engine" << std::endl
       << "M: " << "Switch to Game mode" << std::endl
       << "P: " << "Pause" << std::endl;

  return text.str();
}
