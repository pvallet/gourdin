#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "opengl.h"
#include "shader.h"

class ColoredRectangles {
public:
  ColoredRectangles (glm::vec4 color = glm::vec4(1));
  ColoredRectangles (glm::vec4 color, const std::vector<glm::vec4>& rectangles);
  virtual ~ColoredRectangles();
  ColoredRectangles(ColoredRectangles const&) = delete;
  void operator=   (ColoredRectangles const&) = delete;

  void draw() const;

  void setRectangles(const std::vector<glm::vec4>& rectangles);

  // Loaded in Interface init function to ensure it is only loaded once
  static void loadShader();

private:
  static Shader _plainColorShader;
  static bool _shaderLoaded;

  std::vector<float> _vertices;
  glm::vec4 _color;

  size_t _nbRect;

  GLuint _vao;
  GLuint _vbo;
};
