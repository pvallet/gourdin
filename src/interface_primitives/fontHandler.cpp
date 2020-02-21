#include "fontHandler.h"

#include "texture.h"

#include "vera_38.h"
#define FONT_SIZE_PT 38.f

FontHandler::FontHandler()
  : _fontSize(round(FONT_SIZE_PT / (4.f / 3.f)))
  {

  Texture::flipPixelsUpsideDown(font.tex_width, font.tex_height, 1, font.tex_data);

  // Fill the glyph data with the glyphs from the font
  for (int i = 0; i < font.glyphs_count; i++) {
    char codepoint = font.glyphs[i].codepoint;
    if (codepoint > 0) {
      _glyphs[codepoint].width = font.glyphs[i].width;
      _glyphs[codepoint].height = font.glyphs[i].height;
      _glyphs[codepoint].offset_x = font.glyphs[i].offset_x;
      _glyphs[codepoint].offset_y = font.glyphs[i].offset_y;
      _glyphs[codepoint].advance_x = font.glyphs[i].advance_x;
      _glyphs[codepoint].s0 = font.glyphs[i].s0;
      _glyphs[codepoint].t0 = 1-font.glyphs[i].t0;
      _glyphs[codepoint].s1 = font.glyphs[i].s1;
      _glyphs[codepoint].t1 = 1-font.glyphs[i].t1;

      for (int i = 0; i < 128; i++) {
        _glyphs[codepoint].kerning[i] = 0;
      }

      for (int j = 0; j < font.glyphs[i].kerning_count; j++) {
        _glyphs[codepoint].kerning[font.glyphs[i].kerning[j].codepoint] = font.glyphs[i].kerning[j].kerning;
      }
    }
  }
}

const Glyph* FontHandler::getGlyph(char character) const {
  if (character < 0)
    return nullptr;

  return &_glyphs.at(character);
}

unsigned int FontHandler::getWidth() const {
  return font.tex_width;
}

unsigned int FontHandler::getHeight() const {
  return font.tex_height;
}

unsigned char* FontHandler::getPixelData() const {
  return font.tex_data;
}
