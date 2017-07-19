#pragma once

#include "game.h"

class GameGame : public Game {
public:
  GameGame (SDL2pp::Window& window, Engine& engine);

  void render() const;

  bool genTribe();
  void deleteTribe();
  void changeFocusInDirection(glm::vec2 direction);
  // Changes the character to the one on which the player has clicked if so, otherwise returns focusedCharacter
  void moveCharacter(glm::ivec2 screenTarget);

  inline glm::vec2 getFocusedPos() const {if (!_focusedCharacter) return glm::vec2(0,0);
    return _focusedCharacter->getPos();}
  inline float getCharacterHeight() const {if (!_focusedCharacter) return 0;
    return _focusedCharacter->getSize().y;}

  inline void setTarget(glm::vec2 target) {if (_focusedCharacter) _focusedCharacter->setTarget(target);}
  inline void stopMoving() {if (_focusedCharacter) _focusedCharacter->stop();}

  inline void setPovCamera(bool povCamera) {_povCamera = povCamera; _interface.setTextTopLeft(getInfoText());}
  inline bool getPovCamera() const {return _povCamera;}

private:
  std::string getInfoText() const;

  Controllable* _focusedCharacter;
  std::vector<Controllable*> _tribe;

  bool _povCamera;
};
