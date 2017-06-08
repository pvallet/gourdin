#pragma once

#include "engine.h"
#include "interface.h"

class GameSandbox {
public:
  GameSandbox (Engine& engine, Interface& interface);

  void moveSelection(sf::Vector2i screenPos);
  void goBackToSelection();
  void makeLionsRun();
  void killLion();

  inline Interface& getInterface() const {return _interface;}

  inline void moveCamera(sf::Vector2f newPos) {_engine.moveCamera(newPos);}
  inline void switchWireframe() {_engine.switchWireframe();}
  inline void select(sf::IntRect rect, bool add) {_engine.select(rect,add);}

private:
  Engine& _engine;
  Interface& _interface;
};
