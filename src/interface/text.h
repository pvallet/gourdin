#pragma once

#include <glm/glm.hpp>

#include "fontHandler.h"
#include "opengl.h"
#include "shader.h"

#include <string>

class Text {
public:
  Text ();
  ~Text();
	Text(Text const&)           = delete;
	void operator=(Text const&) = delete;

  inline GLuint getTexID() const {return _texID;}

  void setText(const std::string &str, float fontSize = _fontHandler.getFontSize());
  void setPosition(size_t X, size_t Y);
  void render() const;

  void displayAtlas(glm::uvec2 windowCoords) const;

  glm::uvec2 getSize() const;

  // Loaded in Interface init function to ensure it is only loaded once
  static void loadShader();

private:
  static FontHandler _fontHandler;
  static Shader _textShader;
  static bool _shaderLoaded;

  GLuint _vao;
  GLuint _vbo;
  GLuint _texID;

  size_t _stringLength;
  glm::vec2 _origin;
  glm::vec2 _bounds;
};
