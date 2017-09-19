#pragma once

#include <SDL2pp/SDL2pp.hh>

#include <string>

class LoadingScreen {
public:
  LoadingScreen (SDL2pp::Window& window);

  void updateAndRender(std::string task, float progress);

private:
  SDL2pp::Window& _window;
};
