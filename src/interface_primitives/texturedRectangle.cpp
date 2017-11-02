#include "texturedRectangle.h"

#include "coordConversion.h"
#include "utils.h"

Shader TexturedRectangle::_2DShader;
bool TexturedRectangle::_shaderLoaded = false;

TexturedRectangle::TexturedRectangle (const Texture* texture, glm::ivec4 rect)
	: _textureRect(rect)
	, _texture(texture)
{
	loadShader();
	
	glm::vec4 floatRect = ut::windowRectCoordsToGLRectCoords(rect);

	_verticesAndCoord  = {
     floatRect.x,               floatRect.y,   0, 0,
		 floatRect.x + floatRect.z, floatRect.y,   1, 0,
     floatRect.x,               floatRect.y + floatRect.w, 0, 1,
     floatRect.x + floatRect.z, floatRect.y + floatRect.w, 1, 1
	};

	_vao.bind();
  _vbo.bind();

	glBufferData(	GL_ARRAY_BUFFER, sizeof(_verticesAndCoord), &_verticesAndCoord[0], GL_STATIC_DRAW);

  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);

	VertexBufferObject::unbind();
	VertexArrayObject::unbind();
}

void TexturedRectangle::loadShader() {
  if (!TexturedRectangle::_shaderLoaded) {
    TexturedRectangle::_2DShader.load("src/shaders/2D_shaders/2D.vert", "src/shaders/2D_shaders/simpleTexture.frag");
    TexturedRectangle::_shaderLoaded = true;
  }
}

void TexturedRectangle::draw() const {
	_vao.bind();
	_texture->bind();

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  Texture::unbind();
	VertexArrayObject::unbind();
}
