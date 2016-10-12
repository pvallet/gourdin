#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <thread>

#include "controller.h"

#define DEFAULT_SCREENWIDTH 800
#define DEFAULT_SCREENHEIGHT 600


int main() {
  sf::ContextSettings context(24, 8, 4, 3, 0);

  sf::RenderWindow window(sf::VideoMode::getFullscreenModes().front(), "OpenGL", sf::Style::Fullscreen, context);

  glewInit();

  window.setVerticalSyncEnabled(true);
  window.setActive(true);

  glEnable(GL_DEPTH_TEST);
  glCullFace (GL_BACK);
  glEnable (GL_CULL_FACE);

  Controller controller(&window);

  controller.init();

  controller.run();

  return 0;
}
