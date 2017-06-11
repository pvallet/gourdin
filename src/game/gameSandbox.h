#pragma once

#include "engine.h"
#include "interface.h"

class GameSandbox {
public:
  GameSandbox (sf::RenderWindow& window, Engine& engine, Interface& interface);

  void update(sf::Time elapsed);
  void render() const;

  void select(glm::ivec4 rect, bool add);
  void moveSelection(glm::ivec2 screenTarget);
  void goBackToSelection();
  void makeLionsRun();
  void killLion();
  void clearLog() const;
  void benchmark();

  inline Interface& getInterface() const {return _interface;}

  inline void moveCamera(glm::vec2 newPos) {_engine.moveCamera(newPos);}
  inline void switchLog() {_displayLog = !_displayLog; clearLog();}
  inline void switchWireframe() {_engine.switchWireframe();}
  inline void setScrollSpeedToSlow(bool scrollSpeedSlow) {_scrollSpeedSlow = scrollSpeedSlow;}
  inline bool getScrollSpeedSlow() const {return _scrollSpeedSlow;}

private:
  std::string getInfoText() const;

  bool _displayLog;
  bool _scrollSpeedSlow;

  std::set<Lion*> _selection;

  sf::RenderWindow& _window;
  Engine& _engine;
  Interface& _interface;
};
