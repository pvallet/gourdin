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

  void setText(const std::string &str, float fontSize);
  void setPosition(size_t X, size_t Y);
  void draw() const;

  void displayAtlas(glm::uvec2 windowCoords) const;

  glm::uvec2 getSize() const;

  // Loaded in Interface init function to ensure it is only loaded once
  static void loadShader();

private:
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
