#pragma once

#include "engine.h"
#include "interface.h"

class GameGame {
public:
  GameGame (sf::RenderWindow& window, Engine& engine, Interface& interface);

  void update(sf::Time elapsed);
  void render() const;

  bool genTribe();
  void changeFocusInDirection(sf::Vector2f direction);

  inline sf::Vector2f getFocusedPos() const {return _focusedCharacter->getPos();}
  inline float getCharacterHeight() const {return _focusedCharacter->getSize().y;}

  inline void setTarget(sf::Vector2f target) {_focusedCharacter->setTarget(target);}
  inline void stopMoving() {_focusedCharacter->stop();}
  inline void moveCharacter(sf::Vector2i screenTarget) {
    _focusedCharacter = _engine.moveCharacter(screenTarget, _focusedCharacter);}

  inline void setPovCamera(bool povCamera) {_povCamera = povCamera;}
  inline bool getPovCamera() const {return _povCamera;}

  inline const Engine& getEngine() const {return _engine;}

private:
  std::string getInfoText() const;

  Controllable* _focusedCharacter;

  bool _povCamera;

  sf::RenderWindow& _window;
  Engine& _engine;
  Interface& _interface;
};
