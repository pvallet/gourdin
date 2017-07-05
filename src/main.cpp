#include <GL/glew.h>
#include <SDL_image.h>
#include <SDL2pp/SDL2pp.hh>

#include <cstring>

#include "controller.h"

#ifndef NDEBUG
  #include "testHandler.hpp"
#endif

int main(int argc, char* argv[]) try {

#ifndef NDEBUG
  bool tests = false;
  Clock beginningOfProg;
  TestHandler testHandler(beginningOfProg);

  if (argc > 1) {
    if (strcmp(argv[1],"clean") == 0) {
      testHandler.clean();
      return 0;
    }
    else if (strcmp(argv[1],"tests") == 0)
      tests = true;
    else {
      std::cout << "Unkown option, try 'clean' or 'tests'" << '\n';
      return 0;
    }
  }
#else
  (void) argc;
  (void) argv;
#endif

  SDL_GLContext glContext(0);

  SDL2pp::SDL sdl(SDL_INIT_VIDEO);
  SDL2pp::SDLImage image(IMG_INIT_PNG);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  SDL2pp::Window window("gourdin", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    1366, 768, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

  glContext = SDL_GL_CreateContext(window.Get());

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
  if (tests) {
    testHandler.runTests(controller);
    return 0;
  }
#endif

  controller.run();

  SDL_GL_DeleteContext(glContext);

  return 0;
}
catch (SDL2pp::Exception& e) {
  std::cerr << "Error in: " << e.GetSDLFunction() << std::endl;
  std::cerr << "  Reason: " << e.GetSDLError() << std::endl;
  return 1;
}
catch (std::exception& e) {
  std::cerr << e.what() << std::endl;
  return 1;
}
