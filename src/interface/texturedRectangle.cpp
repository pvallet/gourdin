#include "texturedRectangle.h"

#include "utils.h"

Shader TexturedRectangle::_2DShader;
bool TexturedRectangle::_shaderLoaded = false;

TexturedRectangle::TexturedRectangle (const Texture* texture, float x, float y, float w, float h):
	_verticesAndCoord {
     x,   y,   0, 0,
		 x+w, y,   1, 0,
     x,   y+h, 0, 1,
     x+w, y+h, 1, 1
	},
	_texture(texture) {

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

TexturedRectangle::TexturedRectangle (const Texture* texture, glm::vec4 rect):
	TexturedRectangle(texture, rect.x, rect.y, rect.z, rect.w) {}

TexturedRectangle::~TexturedRectangle() {
	glDeleteBuffers(1, &_vbo);
  glDeleteVertexArrays(1, &_vao);
}

void TexturedRectangle::loadShader() {
  if (!TexturedRectangle::_shaderLoaded) {
    TexturedRectangle::_2DShader.load("src/shaders/2D_shaders/2D.vert", "src/shaders/2D_shaders/simpleTexture.frag");
    TexturedRectangle::_shaderLoaded = true;
  }
}

void TexturedRectangle::draw() const {
	_2DShader.bind();
	glBindVertexArray(_vao);
	_texture->bind();

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  Texture::unbind();
	glBindVertexArray(0);
	Shader::unbind();
}

glm::vec4 TexturedRectangle::getTextureRect() const {
	return glm::vec4(
		_verticesAndCoord[0],
		_verticesAndCoord[1],
		_verticesAndCoord[12] - _verticesAndCoord[0],
		_verticesAndCoord[13] - _verticesAndCoord[1]
	);
}
