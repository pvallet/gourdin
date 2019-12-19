#pragma once

#include "engine.h"
#include "igLayout.h"
#include "popupMenu.h"
#include "loadingScreen.h"

class Game {
public:
  Game (Engine& engine);

  // ---------------------------- Common ---------------------------------------

  void init(LoadingScreen& loadingScreen);
  void mainLoop();

  void update(int msElapsed);
  void render() const;

  inline void switchLog() {_displayLog = !_displayLog;}
  inline       igLayout& getInterface()       {return _interface;}
  inline const igLayout& getInterface() const {return _interface;}
  inline const Engine& getEngine() const {return _engine;}

  inline bool isViewLocked() const {return _lockedView;}
  void setLockedView(bool lockedView);

  // Returns true if there is a controllable element containing screenTarget
  bool pickCharacter(glm::ivec2 screenTarget);

  // -------------------------- Locked view ------------------------------------

  void changeFocusInDirection(glm::vec2 direction);

  inline bool hasFocusedCharacter() const {return _focusedCharacter != nullptr;}
  inline glm::vec2 getFocusedPos() const {if (!_focusedCharacter) return glm::vec2(0,0);
    return _focusedCharacter->getPos();}
  inline float getCharacterHeight() const {if (!_focusedCharacter) return 0;
    return _focusedCharacter->getSize().y;}
  inline const igMovingElement* getFocusedCharacter() const { return _focusedCharacter; }

  inline void setTarget(glm::vec2 target) {if (_focusedCharacter) _focusedCharacter->setTarget(target);}
  inline void stopMoving() {if (_focusedCharacter) _focusedCharacter->stop();}
  void moveFocused(glm::ivec2 screenTarget, bool perpetualMotion = false);

  inline void setPovCamera(bool povCamera) {_povCamera = povCamera; _interface.setTextTopLeft(getInfoTextLockedView());}
  inline bool getPovCamera() const {return _povCamera;}

  // -------------------------- Global view ------------------------------------

  void select(glm::ivec4 rect, bool add);
  void selectAllLions();
  inline void unselect() {select(glm::ivec4(-1,-1,0,0), false);}
  void createLion(glm::ivec2 screenTarget);
  void createAntilope(glm::ivec2 screenTarget);
  void moveSelection(glm::ivec2 screenTarget);
  void goBackToSelection();
  void killLion();
  void benchmark();

  void genTribe();
  void genTribe(glm::ivec2 screenTarget);
  void deleteTribe();

  inline void switchWireframe() {_engine.switchWireframe();}
  inline void setScrollSpeedToSlow(bool scrollSpeedSlow) {_scrollSpeedSlow = scrollSpeedSlow;}
  inline bool getScrollSpeedSlow() const {return _scrollSpeedSlow;}
  inline bool isSelectionEmpty() const {return _selection.size() == 0;}
  inline void displayInfo(const std::string& error) {_interface.setTextBottomCenter(error, _msCenterTextDisplayDuration);}

  inline PopupMenu& getPopupMenu() {return _popupMenu;}

protected:
  virtual void updateCamera() const;
  std::string getInfoTextCommon() const;
  std::string getInfoTextLockedView() const;
  std::string getInfoTextGlobalView() const;
  void setFocusedCharacter(Controllable* focusedCharacter);

  Controllable* _focusedCharacter;
  std::vector<Controllable*> _tribe;

  bool _lockedView;
  bool _displayLog;

  Engine& _engine;
  igLayout _interface;
  PopupMenu _popupMenu;

  // Locked view

  bool _povCamera;

  // Global view

  bool _scrollSpeedSlow;

  const size_t _maxSimultaneousLions;
  size_t _nbLions;
  size_t _bestScore;
  int _msCenterTextDisplayDuration;

  std::set<Lion*> _selection;
};
