#pragma once

#include <SDL2pp/SDL2pp.hh>

// #include "coloredRectangles.h"
#include "text.h"

#include <string>

class LoadingScreen {
public:
  LoadingScreen (SDL2pp::Window& window);

  void updateAndRender();

  // inline void setLoadingProgress(float progress) {_loadingProgress = progress > 100 ? 100 : progress;}
  // inline void setProgressBarText(std::string progressBarText) {_textProgressBar.setText(progressBarText);}

private:
  float _loadingProgress;

  Text _textCenter;
  // Text _textProgressBar;

  // ColoredRectangles _progressBar;

  SDL2pp::Window& _window;
};
