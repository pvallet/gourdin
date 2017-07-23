#pragma once

#include "opengl.h"

class FrameBufferObject {
public:
  FrameBufferObject ();

  void init(size_t width, size_t height,
    GLenum colorBufferInternalFormat, GLenum colorBufferFormat, GLenum colorBufferType);

  inline GLuint getID() const {return _fboIndex;}
  inline GLuint getTexID() const {return _colorBuffer;}
  inline GLuint getTexIDDepth() const {return _depthBuffer;}

private:
  GLuint _fboIndex;
  GLuint _colorBuffer;
  GLuint _depthBuffer;
};
