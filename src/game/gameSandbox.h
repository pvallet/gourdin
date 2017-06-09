#pragma once

#include "engine.h"
#include "interface.h"

class GameSandbox {
public:
  GameSandbox (sf::RenderWindow& window, Engine& engine, Interface& interface);

  void update(sf::Time elapsed);
  void render() const;

  void select(sf::IntRect rect, bool add);
  void moveSelection(sf::Vector2i screenTarget);
  void goBackToSelection();
  void makeLionsRun();
  void killLion();
  void clearLog() const;
  void benchmark();

  inline Interface& getInterface() const {return _interface;}

  inline void moveCamera(sf::Vector2f newPos) {_engine.moveCamera(newPos);}
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
