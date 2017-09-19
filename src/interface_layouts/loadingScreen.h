#pragma once

#include <SDL2pp/SDL2pp.hh>

#include "clock.h"

#include <string>

class LoadingScreen {
public:
  LoadingScreen (SDL2pp::Window& window);

  void updateAndRender(std::string task, float progress);

private:
  std::string _previousTask;
  Clock _timeSincePreviousTaskWasLaunched;

  SDL2pp::Window& _window;
};
