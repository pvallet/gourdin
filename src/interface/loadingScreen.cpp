#include "loadingScreen.h"

#include "camera.h"

// #include <SDL2/SDL_log.h>

LoadingScreen::LoadingScreen(SDL2pp::Window& window):
  _loadingProgress(0),
  _window(window) {}

void LoadingScreen::updateAndRender() {
  Camera& cam = Camera::getInstance();
  _textCenter.setText("Loading");

  _textCenter.setPosition(cam.getWindowW() / 2 - (_textCenter.getSize().x / 2),
                          cam.getWindowH() / 2 - (_textCenter.getSize().y / 2));

  glViewport(0, 0, (GLint) cam.getWindowW(), (GLint) cam.getWindowH());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  _textCenter.draw();
  // _progressBar.draw();
  // _textProgressBar.draw();

  glViewport(0, 0, (GLint) cam.getW(), (GLint) cam.getH());

  SDL_GL_SwapWindow(_window.Get());
}
