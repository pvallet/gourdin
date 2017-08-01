#pragma once

#include "engine.h"
#include "interface.h"

class Game {
public:
  Game (Engine& engine);

  virtual void init();

  virtual void update(int msElapsed);
  virtual void render() const = 0;

  inline void switchLog() {_displayLog = !_displayLog;}
  inline       Interface& getInterface()       {return _interface;}
  inline const Interface& getInterface() const {return _interface;}
  inline const Engine& getEngine() const {return _engine;}

protected:
  virtual void updateCamera() const;
  virtual std::string getInfoText() const;

  bool _displayLog;

  Engine& _engine;
  Interface _interface;
};
