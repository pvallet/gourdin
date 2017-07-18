#pragma once

#include "opengl.h"

class FrameBufferObject {
public:
  FrameBufferObject ();

  void init(size_t width, size_t height);

  inline GLuint getID() const {return _fboIndex;}
  inline GLuint getTexID() const {return _colorBuffer;}

private:
  GLuint _fboIndex;
  GLuint _colorBuffer;
  GLuint _depthBuffer;
};
