#include "generatedImage.h"

#include <cassert>
#include <cmath>

#include "utils.h"

GeneratedImage::GeneratedImage(std::vector<float> blackNwhitePixels) {
  _size = sqrt(blackNwhitePixels.size());

  assert(blackNwhitePixels.size() == _size*_size);

  _pixels.resize(_size*_size*4);

  for (size_t i = 0; i < _size*_size; i++) {
    _pixels[4*i] = blackNwhitePixels[i]*255;
    _pixels[4*i+1] = blackNwhitePixels[i]*255;
    _pixels[4*i+2] = blackNwhitePixels[i]*255;
    _pixels[4*i+3] = 255;
  }
}

void GeneratedImage::invert() {
  for (size_t i = 0; i < _size*_size; i++) {
    for (size_t j = 0; j < 3; j++) {
      _pixels[4*i+j] = 255 - _pixels[4*i+j];
    }
  }
}

void GeneratedImage::applyConvolutionFilter(std::vector<float> filter) {
  int filterSize = sqrt(filter.size());

  assert(filterSize % 2 == 1);
  assert(filter.size() == filterSize*filterSize);

  std::vector<float> nPixels(4*_size*_size, 0);

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

      for (size_t c = 0; c < 4; c++) {
        nPixels[4*(i*_size + j) + c] += _pixels[4*(curI*_size + curJ) + c] * filter[k*filterSize + l];
      }
    }
    }
  }
  }

  for (size_t i = 0; i < 4*_size*_size; i++) {
    _pixels[i] = nPixels[i];
  }
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
