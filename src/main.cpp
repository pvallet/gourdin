#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <cstring>

#include "controller.h"

#ifndef NDEBUG
  #include "testHandler.hpp"
#endif

int main(int argc, char* argv[]) {

#ifndef NDEBUG
  bool onlyTests = false;
  TestHandler testHandler;

  if (argc > 1) {
    if (strcmp(argv[1],"clean") == 0) {
      testHandler.clean();
      return 0;
    }
    else if (strcmp(argv[1],"tests_only") == 0)
      onlyTests = true;
    else {
      std::cout << "Unkown option, try 'clean' or 'tests_only'" << '\n';
      return 0;
    }
  }
#endif

  sf::ContextSettings context(24, 8, 4, 3, 0);

#ifndef NDEBUG
  sf::RenderWindow window(sf::VideoMode(1366, 768), "OpenGL", sf::Style::Default, context);
#else
  sf::RenderWindow window(sf::VideoMode::getFullscreenModes().front(), "OpenGL", sf::Style::Fullscreen, context);
#endif

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

#ifndef NDEBUG
  testHandler.runTests(controller);

  if (onlyTests)
    return 0;
#endif

  controller.run();

  return 0;
}
