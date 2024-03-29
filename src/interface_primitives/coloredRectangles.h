#pragma once

#include <stddef.h> // size_t
#include <vector>
#include <glm/glm.hpp>

#include "basicGLObjects.h"
#include "shader.h"

class ColoredRectangles {
public:
  ColoredRectangles (glm::vec4 color, bool filled = true);
  ColoredRectangles (glm::vec4 color, const glm::ivec4& rectangle, bool filled = true);
  ColoredRectangles (glm::vec4 color, const std::vector<glm::ivec4>& rectangles, bool filled = true);

  void bindShaderAndDraw() const;

  void setRectangles(const glm::ivec4& rectangle);
  void setRectangles(const std::vector<glm::ivec4>& rectangles);

private:
  static void loadShader();
  
  static Shader _plainColorShader;
  static bool _shaderLoaded;

  // To avoid rounding to the left for the lines
  const float _linesOffset;

  std::vector<float> _vertices;
  glm::vec4 _color;
  bool _filled;

  size_t _nbRect;

  VertexArrayObject _vao;
  VertexBufferObject _vbo;
};
