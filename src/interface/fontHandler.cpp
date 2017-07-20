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
