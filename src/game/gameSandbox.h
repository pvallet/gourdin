#pragma once

#include "engine.h"
#include "interface.h"

class GameSandbox {
public:
  GameSandbox (sf::RenderWindow& window, Engine& engine);

  void init();

  void update(int msElapsed);
  void render() const;

  void select(glm::ivec4 rect, bool add);
  void selectAllLions();
  void createLion(glm::ivec2 screenTarget);
  void moveSelection(glm::ivec2 screenTarget);
  void goBackToSelection();
  void makeLionsRun();
  void switchLionsRun();
  void killLion();
  void clearLog() const;
  void benchmark();
  void interruptHunt();
  void startNewHunt();

  inline       Interface& getInterface()       {return _interface;}
  inline const Interface& getInterface() const {return _interface;}

  inline void moveCamera(glm::vec2 newPos) {_engine.moveCamera(newPos);}
  inline void switchLog() {_displayLog = !_displayLog; clearLog();}
  inline void switchWireframe() {_engine.switchWireframe();}
  inline void setScrollSpeedToSlow(bool scrollSpeedSlow) {_scrollSpeedSlow = scrollSpeedSlow;}
  inline bool getScrollSpeedSlow() const {return _scrollSpeedSlow;}
  inline bool huntHasStarted() const {return _huntHasStarted;}
  inline bool isSelectionEmpty() const {return _selection.size() == 0;}

  inline const Engine& getEngine() const {return _engine;}

private:
  std::string getHuntText() const;
  std::string getInfoText() const;

  bool _displayLog;
  bool _scrollSpeedSlow;

  bool _huntHasStarted;
  const size_t _maxSimultaneousLions;
  size_t _nbLions;
  size_t _bestScore;
  int _msHuntDuration;
  int _msCenterTextDisplayDuration;

  Clock _huntStart;

  std::set<Lion*> _selection;

  sf::RenderWindow& _window;
  Engine& _engine;
  Interface _interface;
};
