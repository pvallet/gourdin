#pragma once

class FontHandler {
public:
  FontHandler ();

  unsigned int getWidth() const;
  unsigned int getHeight() const;
  unsigned char* getPixelData() const;
};
