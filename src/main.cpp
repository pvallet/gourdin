#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include "controller.h"

int main() {
  sf::ContextSettings context(24, 8, 4, 3, 0);

  // sf::RenderWindow window(sf::VideoMode::getFullscreenModes().front(), "OpenGL", sf::Style::Fullscreen, context);
  sf::RenderWindow window(sf::VideoMode(1366, 768), "OpenGL", sf::Style::Default, context);

  window.setVerticalSyncEnabled(true);
  window.setKeyRepeatEnabled(false);

  glewExperimental = true;

  if (glewInit() != GLEW_OK)
    std::cerr << "Failed to initialize GLEW";

  glDepthFunc(GL_LEQUAL);
  glEnable(GL_DEPTH_TEST);
  glCullFace (GL_BACK);
  glEnable (GL_CULL_FACE);

  Controller controller(window);

  controller.init();

  controller.run();

  return 0;
}
