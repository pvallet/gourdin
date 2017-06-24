#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <cstring>

#include "controller.h"

#ifndef NDEBUG
  #include "testHandler.hpp"
#endif

int main(int argc, char* argv[]) {

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

  SDL_Window* window(0);
  SDL_GLContext glContext(0);

  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL2 initialization error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return -1;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  window = SDL_CreateWindow("gourdin", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    1366, 768, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

  if (window == 0) {
    std::cout << "SDL2 window creation error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return -1;
  }

  glContext = SDL_GL_CreateContext(window);

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
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
