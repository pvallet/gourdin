#include "game.h"
#include "camera.h"
#include "log.h"

#include <sstream>

Game::Game (Engine& engine):
  _displayLog(true),
  _engine(engine),
  _interface() {}

void Game::init() {
  _interface.setEngineTexture(_engine.getColorBuffer());
  _interface.init();
  _interface.setTextTopLeft(getInfoText());
}

void Game::updateCamera() const {
  Camera& cam = Camera::getInstance();
  cam.setHeight(_engine.getHeight(cam.getPointedPos()));
  cam.apply();
}

void Game::update(int msElapsed) {
  Log& logText = Log::getInstance();
  logText.clear();
  logText.addFPSandCamInfo();

  if (_displayLog)
    _interface.setTextTopRight(logText.getText());
  else
    _interface.setTextTopRight("");

  if (Clock::isGlobalTimerPaused())
    _interface.setTextCenter("PAUSED", 1);
  updateCamera();
  _engine.update(msElapsed);
}

std::string Game::getInfoText() const {
  std::ostringstream text;

  text << "Esc: " << "Quit engine" << std::endl
       << "M: " << "Switch to Game mode" << std::endl
       << "P: " << "Pause" << std::endl
       << "L: " << "Hide/Display log" << std::endl;

  return text.str();
}
