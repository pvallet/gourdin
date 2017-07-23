#include "text.h"

#include "camera.h"
#include "texturedRectangle.h"

FontHandler Text::_fontHandler;
Shader Text::_textShader;
bool Text::_shaderLoaded = false;

Text::Text() :
  _vao(0),
  _vbo(0),
  _texID(0),
  _stringLength(0) {

  glGenTextures(1, &_texID);
  glBindTexture(GL_TEXTURE_2D, _texID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D( GL_TEXTURE_2D, 0, GL_R8, _fontHandler.getWidth(), _fontHandler.getHeight(),
    0, GL_RED, GL_UNSIGNED_BYTE, _fontHandler.getPixelData());

  glBindTexture(GL_TEXTURE_2D, 0);

	glGenBuffers(1, &_vbo);

	glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Text::~Text() {
	glDeleteBuffers(1, &_vbo);
	glDeleteVertexArrays(1, &_vao);
}

void Text::loadShader() {
  if (!Text::_shaderLoaded) {
    Text::_textShader.load("src/shaders/2D_shaders/2D.vert", "src/shaders/2D_shaders/text.frag");
    Text::_shaderLoaded = true;
  }
}

void Text::setText(const std::string &str, float fontSize) {
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);

  Camera& cam = Camera::getInstance();
  float sx = 2 / (float) cam.getWindowW() * fontSize / _fontHandler.getFontSize();
  float sy = 2 / (float) cam.getWindowH() * fontSize / _fontHandler.getFontSize();

  // Default text location on top left corner of the screen
  float x = -1;
  float y =  1 - _fontHandler.getFontSize() * sy;

  float maxX = -1;

  _stringLength = str.size();
  size_t glyphGLDataSize = 24 * sizeof(float);

  glBufferData(GL_ARRAY_BUFFER, _stringLength * glyphGLDataSize, NULL, GL_DYNAMIC_DRAW);

  for (size_t i = 0; i < str.size(); i++) {
    if (str[i] == '\n') {
      y -= _fontHandler.getFontSize() * sy;
      x = -1;
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

      glBufferSubData(GL_ARRAY_BUFFER, i * glyphGLDataSize , glyphGLDataSize, data);

      x += (glyph->advance_x * sx);
    }

    if (x > maxX)
      maxX = x;
  }

  _bounds.x = maxX;
  _bounds.y = y;

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Text::setPosition(size_t X, size_t Y) {
  glm::uvec2 windowCoords(X,Y);
  Camera& cam = Camera::getInstance();
  _origin = cam.windowCoordsToGLCoords(windowCoords);

  // Substract default origin of the text
  _origin.x += 1;
  _origin.y -= 1;
}

void Text::render() const {
  glUseProgram(_textShader.getProgramID());
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBindVertexArray(_vao);
  glBindTexture(GL_TEXTURE_2D, _texID);

  glUniform2f(glGetUniformLocation(_textShader.getProgramID(), "offset"), _origin.x, _origin.y);

  glDrawArrays(GL_TRIANGLES, 0, 6*_stringLength);

  glDisable(GL_BLEND);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindVertexArray(0);
  glUseProgram(0);
}

void Text::displayAtlas(glm::uvec2 windowCoords) const {
  Camera& cam = Camera::getInstance();
  glm::vec4 glCoords = cam.windowRectCoordsToGLRectCoords(glm::vec4(windowCoords,
    _fontHandler.getWidth(), _fontHandler.getHeight()));

  TexturedRectangle atlas(_texID, glCoords.x, glCoords.y, glCoords.z, glCoords.w);

  glUseProgram(_textShader.getProgramID());
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  atlas.draw();

  glDisable(GL_BLEND);
  glUseProgram(0);
}

glm::uvec2 Text::getSize() const {
  Camera& cam = Camera::getInstance();
  return cam.glCoordsToWindowCoords(_bounds);
}
