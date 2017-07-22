#pragma once

#include <array>
#include <glm/glm.hpp>

#include "opengl.h"

class TexturedRectangle {
public:
  TexturedRectangle (GLuint texID, float x, float y, float w, float h);
  TexturedRectangle (GLuint texID, glm::vec4 rect);
  virtual ~TexturedRectangle();
  TexturedRectangle(TexturedRectangle const&) = delete;
  void operator=   (TexturedRectangle const&) = delete;

  void draw() const;

  glm::vec4 getTextureRect() const;

private:
  std::array<float,16> _verticesAndCoord;

  GLuint _texID;
  GLuint _vao;
  GLuint _vbo;
};
