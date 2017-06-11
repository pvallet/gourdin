#include "gameGame.h"

#include "camera.h"

GameGame::GameGame (sf::RenderWindow& window, Engine& engine, Interface& interface):
  _window(window),
  _engine(engine),
  _interface(interface) {}

void GameGame::update(sf::Time elapsed) {
  _engine.update(elapsed);
}

void GameGame::render() const {
  _engine.render();

#ifndef CORE_PROFILE
  _window.pushGLStates();

  _interface.renderTextTopLeft(getInfoText());

  _window.popGLStates();
#endif

  _window.display();
}

std::string GameGame::getInfoText() const {
  std::ostringstream text;

  text << "Esc: " << "Quit engine" << std::endl
       << "M: " << "Switch to Sandbox mode" << std::endl
       << "1: " << "God camera" << std::endl
       << "2: " << "POV camera" << std::endl;
  if (_povCamera)
    text << "Arrows: " << "Move camera around" << std::endl;
  else
    text << "A-E:  " << "Rotate camera" << std::endl;
  text << "ZQSD: " << "Move focused character" << std::endl
      << "LShift+ZQSD: " << "Change focused character to closest in given direction" << std::endl
      << "(The engine is optimised for AZERTY keyboards)" << std::endl
      << std::endl
      << "Click to move the character in the center" << std::endl
      << "Click on another character to change the focus" << std::endl
      << "Click and drag to rotate the camera" << std::endl;

  return text.str();
}

bool GameGame::genTribe() {
  Camera& cam = Camera::getInstance();
  if (_tribe.size() == 0) {
    _tribe = _engine.genTribe(cam.getPointedPos());
    // If we cannot generate a tribe, we fall back to sandbox mode
    if (_tribe.size() == 0)
      return false;

    _focusedCharacter = _tribe.front();
  }

  return true;
}

void GameGame::changeFocusInDirection(glm::vec2 direction) {
  Camera& cam = Camera::getInstance();
  float threshold = sqrt(2)/2.f;

  Controllable* closestHuman = _focusedCharacter;
  float closestDist = MAX_COORD;

  for (size_t i = 0; i < _tribe.size(); i++) {
    glm::vec2 toChar = _tribe[i]->getPos() - _focusedCharacter->getPos();
    float distance = vu::norm(toChar);

    // Character is in the right direction, with +- M_PI/4 margin
    if (vu::dot(toChar, direction)/distance > threshold) {

      // Checks whether the character is visible on the screen
      glm::ivec4 screenRect = _tribe[i]->getScreenRect();
      if (screenRect.y > (int) cam.getH())
        continue;
      if (screenRect.x > (int) cam.getW())
        continue;
      if (screenRect.y + screenRect.w < 0)
        continue;
      if (screenRect.x + screenRect.z < 0)
        continue;


      if (distance < closestDist) {
        closestDist = distance;
        closestHuman = _tribe[i];
      }
    }
  }

  _focusedCharacter = closestHuman;
}

void GameGame::moveCharacter(glm::ivec2 screenTarget) {
  for (size_t i = 0; i < _tribe.size(); i++) {
    glm::ivec4 spriteRect = _tribe[i]->getScreenRect();

    if (vu::contains(spriteRect,screenTarget)) {
      _focusedCharacter = _tribe[i];
      return;
    }
  }

  _focusedCharacter->setTarget(Engine::get2DCoord(screenTarget));
}
