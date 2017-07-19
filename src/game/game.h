#pragma once

#include <SDL2pp/SDL2pp.hh>

#include "engine.h"
#include "interface.h"
#include "shader.h"

class Game {
public:
  Game (SDL2pp::Window& window, Engine& engine);

  virtual void init();

  virtual void update(int msElapsed);
  virtual void render() const = 0;

  inline       Interface& getInterface()       {return _interface;}
  inline const Interface& getInterface() const {return _interface;}
  inline const Engine& getEngine() const {return _engine;}

protected:
  virtual std::string getInfoText() const;

  Shader _2DShader;

  SDL2pp::Window& _window;
  Engine& _engine;
  Interface _interface;
};
