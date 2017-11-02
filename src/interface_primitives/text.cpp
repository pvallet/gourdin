#include "text.h"

#include "camera.h"
#include "coordConversion.h"
#include "texturedRectangle.h"

#include <vector>

#include <SDL_log.h>

FontHandler Text::_fontHandler;
Shader Text::_textShader;
bool Text::_shaderLoaded = false;

Text::Text() :
  _stringLength(0) {

  loadShader();

  _texture.bind();

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D( GL_TEXTURE_2D, 0, GL_R8, _fontHandler.getWidth(), _fontHandler.getHeight(),
    0, GL_RED, GL_UNSIGNED_BYTE, _fontHandler.getPixelData());

  Texture::unbind();

	_vao.bind();
  _vbo.bind();

  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(0);

	VertexBufferObject::unbind();
	VertexArrayObject::unbind();
}

void Text::loadShader() {
  if (!Text::_shaderLoaded) {
    Text::_textShader.load("src/shaders/2D_shaders/2D.vert", "src/shaders/2D_shaders/text.frag");
    Text::_shaderLoaded = true;
  }
}

void Text::setText(const std::string &str, float fontSize) {
  _vbo.bind();

  Camera& cam = Camera::getInstance();
  float sx = 2 / (float) cam.getWindowW() * fontSize / _fontHandler.getFontSize();
  float sy = 2 / (float) cam.getWindowH() * fontSize / _fontHandler.getFontSize();

  // Default text location on top left corner of the screen
  float x = -1;
  float y =  1 - _fontHandler.getFontSize() * sy;

  float maxX = -1;

  _stringLength = str.size();
  size_t glyphGLDataSize = 24;

  std::vector<float> bufferData(_stringLength * glyphGLDataSize);

  for (size_t i = 0; i < str.size(); i++) {
    if (str[i] == '\n') {
      y -= _fontHandler.getFontSize() * sy;
      x = -1;
      std::vector<float> clearChar(glyphGLDataSize,0);
      VertexBufferObject::cpuBufferSubData(bufferData, i * glyphGLDataSize, glyphGLDataSize, &clearChar[0]);
    }

    else {
      const Glyph* glyph = _fontHandler.getGlyph(str[i]);

      if (!glyph) {
        continue;
      }

      if (i > 0)
        x += glyph->kerning[str[i-1]] * sx;

      float x0 = (float) (x + glyph->offset_x * sx);
      float y0 = (float) (y + glyph->offset_y * sy);
      float x1 = (float) (x0 + glyph->width * sx);
      float y1 = (float) (y0 - glyph->height * sy);

      float s0 = glyph->s0;
      float t0 = glyph->t0;
      float s1 = glyph->s1;
      float t1 = glyph->t1;

      struct {float x, y, s, t;} data[6] = {
        { x0, y0, s0, t0 }, { x0, y1, s0, t1 }, { x1, y1, s1, t1 },
        { x0, y0, s0, t0 }, { x1, y1, s1, t1 }, { x1, y0, s1, t0 }
      };

      VertexBufferObject::cpuBufferSubData(bufferData, i * glyphGLDataSize, glyphGLDataSize, data);

      x += (glyph->advance_x * sx);
    }

    if (x > maxX)
      maxX = x;
  }

  glBufferData(GL_ARRAY_BUFFER, bufferData.size() * sizeof(float), &bufferData[0], GL_DYNAMIC_DRAW);

  _bounds.x = maxX;
  _bounds.y = y;

  VertexBufferObject::unbind();
}

void Text::setPosition(const glm::ivec2& windowCoords) {
  _origin = ut::windowCoordsToGLCoords(windowCoords);

  // Substract default origin of the text
  _origin.x += 1;
  _origin.y -= 1;
}

void Text::bindShaderAndDraw() const {
  _textShader.bind();
  _vao.bind();
  _texture.bind();
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glUniform2fv(_textShader.getUniformLocation("offset"), 1, &_origin[0]);

  glDrawArrays(GL_TRIANGLES, 0, 6*_stringLength);

  glDisable(GL_BLEND);
  Texture::unbind();
  VertexBufferObject::unbind();
  Shader::unbind();
}

void Text::displayAtlas(const glm::ivec2& windowCoords) const {
  glm::ivec4 atlasRect(windowCoords, _fontHandler.getWidth(), _fontHandler.getHeight());

  TexturedRectangle atlas(&_texture, atlasRect);

  float defaultOffset[2] = {0,0};

  _textShader.bind();
  glUniform2fv(_textShader.getUniformLocation("offset"), 1, &defaultOffset[0]);

  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  atlas.draw();

  glDisable(GL_BLEND);
  Shader::unbind();
}

glm::uvec2 Text::getSize() const {
  return ut::glCoordsToWindowCoords(_bounds);
}
