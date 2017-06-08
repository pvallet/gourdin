#include "gameGame.h"

#include "camera.h"

GameGame::GameGame (Engine& engine, Interface& interface):
  _engine(engine),
  _interface(interface) {}

bool GameGame::genTribe() {
  Camera& cam = Camera::getInstance();
  if (_engine.getTribe().size() == 0) {
    _engine.genTribe(cam.getPointedPos());
    // If we cannot generate a tribe, we fall back to sandbox mode
    if (_engine.getTribe().size() == 0)
      return false;

    _focusedCharacter = _engine.getTribe().front();
  }

  return true;
}

void GameGame::changeFocusInDirection(sf::Vector2f direction) {
  Camera& cam = Camera::getInstance();
  std::vector<Controllable*> tribe = _engine.getTribe();
  float threshold = sqrt(2)/2.f;

  Controllable* closestHuman = _focusedCharacter;
  float closestDist = MAX_COORD;

  for (size_t i = 0; i < tribe.size(); i++) {
    sf::Vector2f toChar = tribe[i]->getPos() - _focusedCharacter->getPos();
    float distance = vu::norm(toChar);

    // Character is in the right direction, with +- M_PI/4 margin
    if (vu::dot(toChar, direction)/distance > threshold) {

      // Checks whether the character is visible on the screen
      sf::IntRect screenCoord = tribe[i]->getScreenCoord();
      if (screenCoord.top > (int) cam.getH())
        continue;
      if (screenCoord.left > (int) cam.getW())
        continue;
      if (screenCoord.top + screenCoord.height < 0)
        continue;
      if (screenCoord.left + screenCoord.width < 0)
        continue;


      if (distance < closestDist) {
        closestDist = distance;
        closestHuman = tribe[i];
      }
    }
  }

  _focusedCharacter = closestHuman;
}
