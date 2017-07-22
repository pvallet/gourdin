#pragma once

#include "engine.h"
#include "interface.h"

class Game {
public:
  Game (Engine& engine);

  virtual void init();

  virtual void update(int msElapsed);
  virtual void render() const = 0;

  inline       Interface& getInterface()       {return _interface;}
  inline const Interface& getInterface() const {return _interface;}
  inline const Engine& getEngine() const {return _engine;}

protected:
  virtual std::string getInfoText() const;

  Engine& _engine;
  Interface _interface;
};
