#include "loadingScreen.h"

#include "camera.h"
#include "coloredRectangles.h"
#include "text.h"

LoadingScreen::LoadingScreen(SDL2pp::Window& window):
  _window(window) {}

void LoadingScreen::updateAndRender() {
  Camera& cam = Camera::getInstance();
  Text textCenter;
  textCenter.setText("Loading");

  textCenter.setPosition(cam.getWindowW() / 2 - (textCenter.getSize().x / 2),
                         cam.getWindowH() / 2 - (textCenter.getSize().y / 2));

  glm::vec4 progressBarFrameRect(cam.windowRectCoordsToGLRectCoords(glm::uvec4(
    cam.getWindowW() / 2 - 1.5 * textCenter.getSize().x / 2,
    cam.getWindowH() / 2 + 1.2 * textCenter.getSize().y / 2,
    textCenter.getSize().x * 1.5,
    12 + 4
  )));

  ColoredRectangles progressBarFrame(glm::vec4(0.52, 0.34, 0.138, 1), progressBarFrameRect, false);

  glViewport(0, 0, (GLint) cam.getWindowW(), (GLint) cam.getWindowH());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  textCenter.draw();
  progressBarFrame.draw();

  glViewport(0, 0, (GLint) cam.getW(), (GLint) cam.getH());

  SDL_GL_SwapWindow(_window.Get());
}
