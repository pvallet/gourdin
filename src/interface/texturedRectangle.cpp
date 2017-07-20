#include "texturedRectangle.h"

#include "utils.h"

TexturedRectangle::TexturedRectangle (GLuint texID, float x, float y, float w, float h) :
	_verticesAndCoord {
     x,   y,   0, 0,
		 x+w, y,   1, 0,
     x,   y+h, 0, 1,
     x+w, y+h, 1, 1
	},
	_texID(texID) {

  // vbo
	glGenBuffers(1, &_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);

  glBufferData(	GL_ARRAY_BUFFER, sizeof(_verticesAndCoord), &_verticesAndCoord[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

	// vao

	glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

TexturedRectangle::~TexturedRectangle() {
	glDeleteBuffers(1, &_vbo);
  glDeleteVertexArrays(1, &_vao);
}

void TexturedRectangle::draw() const {
	glBindVertexArray(_vao);
	glBindTexture(GL_TEXTURE_2D, _texID);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}
