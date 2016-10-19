#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <cassert>

#include "controller.h"

#define DEFAULT_SCREENWIDTH 800
#define DEFAULT_SCREENHEIGHT 600


int main() {
  sf::ContextSettings context(24, 8, 4, 3, 0);
  // sf::ContextSettings context(24, 8, 4, 3, 3, sf::ContextSettings::Core);
  // sf::RenderWindow window(sf::VideoMode::getFullscreenModes().front(), "OpenGL", sf::Style::Fullscreen, context);
  sf::RenderWindow window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, context);
  window.setVerticalSyncEnabled(true);
  window.setActive(true);

  glewExperimental = true;

  assert(glewInit() == GLEW_OK && "Failed to initialize GLEW");

  printf("GLSL VERSION: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

  glEnable(GL_DEPTH_TEST);
  glCullFace (GL_BACK);
  glEnable (GL_CULL_FACE);

  Controller controller(&window);

  controller.init();

  controller.run();

  return 0;
}
