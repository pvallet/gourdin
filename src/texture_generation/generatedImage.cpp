#include "generatedImage.h"

#include <cmath>

GeneratedImage::GeneratedImage(std::vector<float> blackNwhitePixels) {
  _size = sqrt(blackNwhitePixels.size());

  _pixels.resize(_size*_size*4);

  for (size_t i = 0; i < _size*_size; i++) {
    _pixels[4*i] = blackNwhitePixels[i]*255;
    _pixels[4*i+1] = blackNwhitePixels[i]*255;
    _pixels[4*i+2] = blackNwhitePixels[i]*255;
    _pixels[4*i+3] = 255;
  }
}
