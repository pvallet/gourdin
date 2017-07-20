#pragma once

#include <glm/glm.hpp>

#include "fontHandler.h"
#include "opengl.h"

#include <string>

class Text {
public:
  Text ();
  ~Text();
	Text(Text const&)           = delete;
	void operator=(Text const&) = delete;

  inline GLuint getTexID() const {return _texID;}

  void renderString(const std::string &str, float x, float y, float sx, float sy) const;

  void displayAtlas(glm::uvec2 windowCoords) const;

private:
  static FontHandler _fontHandler;

  GLuint _vao;
  GLuint _vbo;
  GLuint _texID;
};
