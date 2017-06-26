// This header file is adapted from SFML/OpenGL.hpp

#pragma once

#ifdef _WIN32

    // The Visual C++ version of gl.h uses WINGDIAPI and APIENTRY but doesn't define them
    #ifdef _MSC_VER
        #include <windows.h>
    #endif

    #include <GL/gl.h>

#elif __ANDROID__

  #include <GLES/gl.h>
  #include <GLES/glext.h>

  // We're not using OpenGL ES 2+ yet, but we can use the sRGB extension
  #include <GLES2/gl2ext.h>

#elif __linux__

    // #if defined(SFML_OPENGL_ES)
    //     #include <GLES/gl.h>
    //     #include <GLES/glext.h>
    // #else
        #include <GL/gl.h>
    // #endif

#elif __APPLE__
  #include "TargetConditionals.h"
  #if TARGET_OS_IPHONE

    #include <OpenGLES/ES1/gl.h>
    #include <OpenGLES/ES1/glext.h>

  #elif TARGET_OS_MAC

    #include <OpenGL/gl.h>

  #endif
#endif
