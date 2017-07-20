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

  void renderString(const std::string &str, glm::uvec2 windowCoords) const;

  void displayAtlas(glm::uvec2 windowCoords) const;

  // Loaded in Interface init function to ensure it is only loaded once
  static void loadShader();

private:
  static FontHandler _fontHandler;
  static Shader _textShader;
  static bool _shaderLoaded;

  GLuint _vao;
  GLuint _vbo;
  GLuint _texID;
};
