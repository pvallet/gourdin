#pragma once

#include "engine.h"
#include "interface.h"

class Game {
public:
  Game (Engine& engine);

  // ---------------------------- Common ---------------------------------------

  void init();
  void mainLoop();

  void update(int msElapsed);
  void render() const;

  inline void switchLog() {_displayLog = !_displayLog;}
  inline       Interface& getInterface()       {return _interface;}
  inline const Interface& getInterface() const {return _interface;}
  inline const Engine& getEngine() const {return _engine;}

  inline bool isViewLocked() const {return _lockedView;}
  void setLockedView(bool lockedView);

  // Returns true if there is a controllable element containing screenTarget
  bool pickCharacter(glm::ivec2 screenTarget);
  void makeLionsRun();
  void stopLionsRun();
  void switchLionsRun();

  // -------------------------- Locked view ------------------------------------

  void changeFocusInDirection(glm::vec2 direction);

  inline bool hasFocusedCharacter() const {return _focusedCharacter != nullptr;}
  inline glm::vec2 getFocusedPos() const {if (!_focusedCharacter) return glm::vec2(0,0);
    return _focusedCharacter->getPos();}
  inline float getCharacterHeight() const {if (!_focusedCharacter) return 0;
    return _focusedCharacter->getSize().y;}

  inline void setTarget(glm::vec2 target) {if (_focusedCharacter) _focusedCharacter->setTarget(target);}
  inline void moveFocused(glm::ivec2 screenTarget) {if (_focusedCharacter) _focusedCharacter->setTarget(_engine.get2DCoord(screenTarget));}
  inline void stopMoving() {if (_focusedCharacter) _focusedCharacter->stop();}

  inline void setPovCamera(bool povCamera) {_povCamera = povCamera; _interface.setTextTopLeft(getInfoTextLockedView());}
  inline bool getPovCamera() const {return _povCamera;}

  // -------------------------- Global view ------------------------------------

  void select(glm::ivec4 rect, bool add);
  void selectAllLions();
  inline void unselect() {select(glm::ivec4(-1,-1,0,0), false);}
  void createLion(glm::ivec2 screenTarget);
  void moveSelection(glm::ivec2 screenTarget);
  void goBackToSelection();
  void killLion();
  void benchmark();
  void interruptHunt();
  void startNewHunt();

  void genTribe();
  void deleteTribe();

  inline void switchWireframe() {_engine.switchWireframe();}
  inline void setScrollSpeedToSlow(bool scrollSpeedSlow) {_scrollSpeedSlow = scrollSpeedSlow;}
  inline bool getScrollSpeedSlow() const {return _scrollSpeedSlow;}
  inline bool huntHasStarted() const {return _huntHasStarted;}
  inline bool isSelectionEmpty() const {return _selection.size() == 0;}
  inline void displayInfo(const std::string& error) {_interface.setTextBottomCenter(error, _msCenterTextDisplayDuration);}

protected:
  virtual void updateCamera() const;
  std::string getInfoTextCommon() const;
  std::string getInfoTextLockedView() const;
  std::string getInfoTextGlobalView() const;
  std::string getHuntText() const;
  void setFocusedCharacter(Controllable* focusedCharacter);

  Controllable* _focusedCharacter;
  std::vector<Controllable*> _tribe;

  bool _lockedView;
  bool _displayLog;

  Engine& _engine;
  Interface _interface;

  // Locked view

  bool _povCamera;

  // Global view

  bool _scrollSpeedSlow;

  bool _huntHasStarted;
  const size_t _maxSimultaneousLions;
  size_t _nbLions;
  size_t _bestScore;
  int _msHuntDuration;
  int _msCenterTextDisplayDuration;

  Clock _huntStart;

  std::set<Lion*> _selection;
};
