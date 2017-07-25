#pragma once

#include <array>
#include <glm/glm.hpp>

#include "opengl.h"
#include "shader.h"
#include "texture.h"

class TexturedRectangle {
public:
  TexturedRectangle (const Texture* texture, float x, float y, float w, float h);
  TexturedRectangle (const Texture* texture, glm::vec4 rect);
  virtual ~TexturedRectangle();
  TexturedRectangle(TexturedRectangle const&) = delete;
  void operator=   (TexturedRectangle const&) = delete;

  void draw() const;

  glm::vec4 getTextureRect() const;

  // Loaded in Interface init function to ensure it is only loaded once
  static void loadShader();

private:
  static Shader _2DShader;
  static bool _shaderLoaded;

  std::array<float,16> _verticesAndCoord;

  const Texture* _texture;
  GLuint _vao;
  GLuint _vbo;
};
