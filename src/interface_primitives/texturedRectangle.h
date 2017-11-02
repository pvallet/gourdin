#pragma once

#include <array>
#include <glm/glm.hpp>

#include "basicGLObjects.h"
#include "shader.h"
#include "texture.h"

class TexturedRectangle {
public:
  TexturedRectangle (const Texture* texture, glm::ivec4 rect);

  void draw() const;

  inline glm::ivec4 getTextureRect() const {return _textureRect;}

  static void bindDefaultShader() {_2DShader.bind();}

private:
  static void loadShader();
  
  static Shader _2DShader;
  static bool _shaderLoaded;

  std::array<float,16> _verticesAndCoord;
  glm::ivec4 _textureRect;

  const Texture* _texture;
  VertexArrayObject _vao;
  VertexBufferObject _vbo;
};
