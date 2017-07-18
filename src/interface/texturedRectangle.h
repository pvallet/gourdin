#pragma once

#include <array>

#include "opengl.h"

class TexturedRectangle {
public:
  TexturedRectangle (GLuint texID, float x, float y, float w, float h);
  virtual ~TexturedRectangle();
  TexturedRectangle(TexturedRectangle const&) = delete;
  void operator=   (TexturedRectangle const&) = delete;

  void draw() const;

private:
  std::array<float,8> _vertices;
  std::array<float,8> _coord;

  GLuint _texID;
  GLuint _vao;
  GLuint _vbo;
};
