#pragma once

#include <array>
#include <memory>

struct Glyph {
  int width, height;
  int offset_x, offset_y;
  float advance_x;
  float s0, t0, s1, t1;
  std::array<float, 128> kerning;
};

class FontHandler {
public:
  FontHandler ();

  unsigned int getWidth() const;
  unsigned int getHeight() const;
  unsigned char* getPixelData() const;

  const Glyph* getGlyph(char character) const;

private:
  std::array<std::unique_ptr<Glyph>,128> _glyphs;
};
