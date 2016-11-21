#include "utils.h"

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include <iostream>
#include <string>

bool Check_gl_error(const char *file, int line) {
  GLenum err (glGetError());

  bool isError = false;

  while(err != GL_NO_ERROR) {
    std::string error;

    switch(err) {
      case GL_INVALID_OPERATION:             error="INVALID_OPERATION";      break;
      case GL_INVALID_ENUM:                  error="INVALID_ENUM";           break;
      case GL_INVALID_VALUE:                 error="INVALID_VALUE";          break;
      case GL_OUT_OF_MEMORY:                 error="OUT_OF_MEMORY";          break;
      case GL_INVALID_FRAMEBUFFER_OPERATION: error="INVALID_FRAMEBUFFER_OPERATION";  break;
      case GL_STACK_UNDERFLOW:               error="STACK_UNDERFLOW";        break;
      case GL_STACK_OVERFLOW:                error="STACK_OVERFLOW";         break;
    }

    std::cerr << "GL_" << error << " - " << file << ":" << line << std::endl;
    err = glGetError();
    isError = true;
  }

  return isError;
}
