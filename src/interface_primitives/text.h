#pragma once

#include <glm/glm.hpp>

#include "basicGLObjects.h"
#include "fontHandler.h"
#include "shader.h"
#include "texture.h"

#include <string>

class Text {
public:
  Text ();

  void setText(const std::string &str, float fontSize, float leading = 1.47);
  void setPosition(const glm::ivec2& windowCoords);
  void bindShaderAndDraw() const;

  void displayAtlas(const glm::ivec2& windowCoords) const;

  glm::uvec2 getSize() const;

private:
  static void loadShader();

  static FontHandler _fontHandler;
  static Shader _textShader;
  static bool _shaderLoaded;

  VertexArrayObject _vao;
  VertexBufferObject _vbo;
  Texture _texture;

  size_t _stringLength;
  glm::vec2 _origin;
  glm::vec2 _bounds;
};
