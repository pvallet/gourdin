#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <cassert>

#include "controller.h"

int main() {
  sf::ContextSettings context(24, 8, 4, 3, 0);

  sf::RenderWindow window;

  // Fix for SFML 2.3.2, where the viewport is sometimes translated downwards
  for (size_t i = 0; i < 20; i++) {
    window.create(sf::VideoMode::getFullscreenModes().front(), "OpenGL", sf::Style::Fullscreen, context);
    if (window.getSize().y == sf::VideoMode::getFullscreenModes().front().height)
      break;
  }

  // sf::RenderWindow window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, context);

  window.setVerticalSyncEnabled(true);
  window.setActive(true);

  glewExperimental = true;

  assert(glewInit() == GLEW_OK && "Failed to initialize GLEW");

  glEnable(GL_DEPTH_TEST);
  glCullFace (GL_BACK);
  glEnable (GL_CULL_FACE);

  Controller controller(window);

  controller.init();

  controller.run();

  return 0;
}
