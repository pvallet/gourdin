#include "generatedImage.h"

#include <cassert>
#include <cmath>

#include "utils.h"

GeneratedImage::GeneratedImage(std::vector<float> pixels) {
  _size = sqrt(pixels.size());

  assert(pixels.size() == _size*_size);

  _pixels = pixels;
}

void GeneratedImage::invert() {
  for (size_t i = 0; i < _size*_size; i++) {
    _pixels[i] = 1 - _pixels[i];
  }
}

void GeneratedImage::applyConvolutionFilter(std::vector<float> filter) {
  int filterSize = sqrt(filter.size());

  assert(filterSize % 2 == 1);
  assert(filter.size() == filterSize*filterSize);

  std::vector<float> nPixels(_size*_size, 0);

  for (int i = 0; i < _size; i++) {
  for (int j = 0; j < _size; j++) {
    for (int k = 0; k < filterSize; k++) {
    for (int l = 0; l < filterSize; l++) {
      int curI = i - filterSize/2 + k;
      int curJ = j - filterSize/2 + l;

      if (curI < 0)
        curI += _size;
      if (curJ < 0)
        curJ += _size;
      if (curI >= _size)
        curI -= _size;
      if (curJ >= _size)
        curJ -= _size;

      nPixels[i*_size + j] += _pixels[curI*_size + curJ] * filter[k*filterSize + l];
    }
    }
  }
  }

  _pixels = nPixels;
}

std::vector<float> GeneratedImage::generateBoxFilter(size_t size) {
  if (size % 2 == 0)
    size--;

  std::vector<float> filter(size*size, 0);

  for (size_t i = 0; i < size*size; i++) {
    filter[i] = 1.f / (float) (size*size);
  }

  return filter;
}

std::vector<float> GeneratedImage::generateGaussianFilter(size_t size, float sigma) {
  if (size % 2 == 0)
    size--;

  std::vector<float> filter(size*size, 0);

  sigma *= size;
  float s = 2.f * sigma * sigma;
  int halfSize = size/2;

  // sum is for normalization
  float sum = 0.f;

  for (int i = -halfSize; i <= halfSize; i++) {
    for (int j = -halfSize; j <= halfSize; j++) {
      float r2 = i*i + j*j;

      size_t index = (i + halfSize) * size + j + halfSize;
      filter[index] = exp(-r2/s);
      sum += filter[index];
    }
  }

  // normalize the filter
  for (size_t i = 0; i < size*size; i++) {
    filter[i] /= sum;
  }

  return filter;
}
