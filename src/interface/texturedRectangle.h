#pragma once

#include <array>
#include <glm/glm.hpp>

#include "basicGLObjects.h"
#include "shader.h"
#include "texture.h"

class TexturedRectangle {
public:
  TexturedRectangle (const Texture* texture, float x, float y, float w, float h);
  TexturedRectangle (const Texture* texture, glm::vec4 rect);

  void draw() const;

  glm::vec4 getTextureRect() const;

  // Loaded in Interface init function to ensure it is only loaded once
  static void loadShader();

private:
  static Shader _2DShader;
  static bool _shaderLoaded;

  std::array<float,16> _verticesAndCoord;

  const Texture* _texture;
  VertexArrayObject _vao;
  VertexBufferObject _vbo;
};
