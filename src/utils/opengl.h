#pragma once

#ifdef _WIN32

  #ifdef _MSC_VER
    #include <windows.h>
  #endif

  #define GL_GLEXT_PROTOTYPES
  #include <GL/gl.h>

#elif __ANDROID__

  #include <GLES3/gl3.h>

#elif __linux__

  #define GL_GLEXT_PROTOTYPES
  #include <GL/gl.h>

#elif __APPLE__
  #include "TargetConditionals.h"
  #if TARGET_OS_IPHONE

    #include <OpenGLES/ES3/gl3.h>

  #elif TARGET_OS_MAC

    #define GL_GLEXT_PROTOTYPES
    #include <OpenGL/gl.h>

  #endif
#endif
