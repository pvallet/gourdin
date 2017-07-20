#include "text.h"

#include "camera.h"
#include "texturedRectangle.h"

FontHandler Text::_fontHandler;

Text::Text() :
  _vao(0),
  _vbo(0),
  _texID(0) {

  glGenTextures(1, &_texID);
  glBindTexture(GL_TEXTURE_2D, _texID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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

#include <iostream>

void Text::renderString(
      const std::string &str,
      float x,
      float y,
      float sx,
      float sy) const {

  glBindVertexArray(_vao);
  // glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBindTexture(GL_TEXTURE_2D, _texID);
//    for (int i = 0; i < str.size(); i++) {
// //Find the glyph for the character we are looking for
//       texture_glyph_t *glyph = 0;
//       for (int j = 0; j < font.glyphs_count; ++j) {
//          if (font.glyphs[j].codepoint == str[i]) {
//             glyph = &font.glyphs[j];
//             break;
//          }
//       }
//       if (!glyph) {
//          continue;
//       }
//       // x += glyph->kerning[0].kerning;
//       float x0 = (float) (x + glyph->offset_x * sx);
//       float y0 = (float) (y + glyph->offset_y * sy);
//       float x1 = (float) (x0 + glyph->width * sx);
//       float y1 = (float) (y0 - glyph->height * sy);
//
//       float s0 = glyph->s0;
//       float t0 = glyph->t0;
//       float s1 = glyph->s1;
//       float t1 = glyph->t1;
//
//       struct {float x, y, s, t;} data[6] = { { x0, y0, s0, t0 }, { x0, y1, s0, t1 }, { x1, y1, s1, t1 }, { x0, y0, s0, t0 }, { x1, y1, s1, t1 }, { x1, y0, s1, t0 } };
//
//       glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), data, GL_DYNAMIC_DRAW);
//
//       glDrawArrays(GL_TRIANGLES, 0, 6);
//       x += (glyph->advance_x * sx);
//    }

   glBindTexture(GL_TEXTURE_2D, 0);
  //  glBindBuffer(GL_ARRAY_BUFFER, 0);
 	glBindVertexArray(0);
}

void Text::displayAtlas(glm::uvec2 windowCoords) const {
  Camera& cam = Camera::getInstance();
  glm::vec4 glCoords = cam.rectWindowCoordsToGLCoords(glm::vec4(windowCoords,
    _fontHandler.getWidth(), _fontHandler.getHeight()));

  TexturedRectangle atlas(_texID, glCoords.x, glCoords.y, glCoords.z, glCoords.w);

  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  atlas.draw();

  glDisable(GL_BLEND);
}
