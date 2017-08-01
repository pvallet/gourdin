#include "gameGame.h"

#include "camera.h"
#include "opengl.h"
#include "texturedRectangle.h"

#include <sstream>

GameGame::GameGame (Engine& engine):
  Game::Game(engine),
  _povCamera(false) {}

void GameGame::render() const {
  Camera& cam = Camera::getInstance();

  _engine.renderToFBO();

  glViewport(0, 0, (GLint) cam.getWindowW(), (GLint) cam.getWindowH());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  _interface.renderEngine();
  _interface.renderText();
  glViewport(0, 0, (GLint) cam.getW(), (GLint) cam.getH());
}

std::string GameGame::getInfoText() const {
  std::ostringstream text;

  text << "1: " << "God camera" << std::endl
       << "2: " << "POV camera" << std::endl;
  if (_povCamera)
    text << "Arrows: " << "Move camera around" << std::endl;
  else
    text << "A-E:  " << "Rotate camera" << std::endl;
  text << "WASD: " << "Move focused character" << std::endl
      << "LShift+WASD: " << "Change focused character to closest in given direction" << std::endl
      << std::endl
      << "Click to move the character in the center" << std::endl
      << "Click on another character to change the focus" << std::endl
      << "Click and drag to rotate the camera" << std::endl;

  return Game::getInfoText() + text.str();
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

void GameGame::deleteTribe() {
  std::vector<igMovingElement*> tribe(_tribe.size());

  #pragma omp parallel for
  for (size_t i = 0; i < _tribe.size(); i++) {
    tribe[i] = (igMovingElement*) _tribe[i];
  }

  _engine.deleteElements(tribe);
  _tribe.clear();
  _focusedCharacter = nullptr;
}

void GameGame::changeFocusInDirection(glm::vec2 direction) {
  Camera& cam = Camera::getInstance();
  float threshold = sqrt(2)/2.f;

  Controllable* closestHuman = _focusedCharacter;
  float closestDist = MAX_COORD;

  for (size_t i = 0; i < _tribe.size(); i++) {
    glm::vec2 toChar = _tribe[i]->getPos() - _focusedCharacter->getPos();
    float distance = glm::length(toChar);

    // Character is in the right direction, with +- M_PI/4 margin
    if (glm::dot(toChar, direction)/distance > threshold) {

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

    if (ut::contains(spriteRect,screenTarget)) {
      _focusedCharacter = _tribe[i];
      return;
    }
  }

  if (_focusedCharacter != nullptr)
    _focusedCharacter->setTarget(_engine.get2DCoord(screenTarget));
}
