#include "fontHandler.h"

#include <algorithm>
#include "vera_mono_10.h"

FontHandler::FontHandler() {
  // Flip the texture upside down to match OpenGL coordinates system
  for (unsigned int i = 0; i < font.tex_width / 2; i++) {
    for (unsigned int j = 0; j < font.tex_height; j++) {
      std::swap(font.tex_data[i * font.tex_width + j],
        font.tex_data[font.tex_width * (font.tex_width - i - 1) + j]);
    }
  }

  // Fill the glyph data with the glyphs from the font
  for (size_t i = 0; i < font.glyphs_count; i++) {
    char codepoint = font.glyphs[i].codepoint;
    _glyphs[codepoint].reset(new Glyph);

    _glyphs[codepoint]->width = font.glyphs[i].width;
    _glyphs[codepoint]->height = font.glyphs[i].height;
    _glyphs[codepoint]->offset_x = font.glyphs[i].offset_x;
    _glyphs[codepoint]->offset_y = font.glyphs[i].offset_y;
    _glyphs[codepoint]->advance_x = font.glyphs[i].advance_x;
    _glyphs[codepoint]->s0 = font.glyphs[i].s0;
    _glyphs[codepoint]->t0 = 1-font.glyphs[i].t0;
    _glyphs[codepoint]->s1 = font.glyphs[i].s1;
    _glyphs[codepoint]->t1 = 1-font.glyphs[i].t1;

    for (size_t i = 0; i < 128; i++) {
      _glyphs[codepoint]->kerning[i] = 0;
    }

    for (size_t j = 0; j < font.glyphs[i].kerning_count; j++) {
      _glyphs[codepoint]->kerning[font.glyphs[i].kerning[j].codepoint] = font.glyphs[i].kerning[j].kerning;
    }
  }
}

const Glyph* FontHandler::getGlyph(char character) const {
  if (character < 0)
    return nullptr;

  return _glyphs.at(character).get();
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
