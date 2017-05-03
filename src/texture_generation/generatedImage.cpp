#include "generatedImage.h"

#include <SFML/Graphics.hpp>

#include <cassert>
#include <cmath>
#include <iostream>

#include "utils.h"

GeneratedImage::GeneratedImage() :
  _size(0) {}

GeneratedImage::GeneratedImage(std::vector<float> pixels) {
  setPixels(pixels);
}

void GeneratedImage::setPixels(const std::vector<float>& pixels) {
  _size = sqrt(pixels.size());

  assert(pixels.size() == _size*_size);

  _pixels = pixels;
}

bool GeneratedImage::loadFromFile(std::string filename) {
  sf::Image img;
  if (!img.loadFromFile(filename))
    return false;

  if (img.getSize().x != img.getSize().y) {
    std::cerr << "Error in GeneratedImage::loadFromFile: " << filename << " is not square." << '\n';
    return false;
  }

  const sf::Uint8* imgPixels = img.getPixelsPtr();
  _size = img.getSize().x;
  _pixels.resize(_size*_size);

  for (size_t i = 0; i < _size*_size; i++) {
    _pixels[i] = imgPixels[4*i] / 255.f;
  }

  return true;
}

void GeneratedImage::saveToFile(std::string filename) const {
  std::vector<sf::Uint8> rgbPixels(4*_pixels.size());

  for (size_t i = 0; i < _pixels.size(); i++) {
    for (size_t j = 0; j < 3; j++) {
      rgbPixels[4*i + j] = _pixels[i] * 255;
    }
    rgbPixels[4*i + 3] = 255;
  }

  sf::Texture texture;
	texture.create(_size, _size);
	texture.update(&rgbPixels[0]);

	texture.copyToImage().saveToFile(filename);
}

void GeneratedImage::invert() {
  for (size_t i = 0; i < _size*_size; i++) {
    _pixels[i] = 1 - _pixels[i];
  }
}

void GeneratedImage::applyConvolutionFilter(const std::vector<float>& filter) {
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
      else if (curI >= _size)
        curI -= _size;

      if (curJ < 0)
        curJ += _size;
      else if (curJ >= _size)
        curJ -= _size;

      nPixels[i*_size + j] += _pixels[curI*_size + curJ] * filter[k*filterSize + l];
    }
    }
  }
  }

  _pixels = nPixels;
}

void GeneratedImage::multiply(const std::vector<float>& img) {
  assert(img.size() == _pixels.size());

  for (size_t i = 0; i < _pixels.size(); i++) {
    _pixels[i] *= img[i];
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

float GeneratedImage::cubicInterpolate(float before_p0, float p0, float p1, float after_p1, float t) {
  float a3 = -0.5*before_p0 + 1.5*p0 - 1.5*p1 + 0.5*after_p1;
  float a2 = before_p0 - 2.5*p0 + 2*p1 - 0.5*after_p1;
  float a1 = -0.5*before_p0 + 0.5*p1;
  float a0 = p0;

  return (a3 * t*t*t) + (a2 * t*t) + (a1 * t) + a0;
}

// Interpolation of a line for a fixed y
float GeneratedImage::bicubicFirstDim(int intX, int intY, float fracX, const std::vector<float>& pixels) {
  int size = sqrt(pixels.size());

  // We make intX and intY positive so that we can take them modulo size to keep them whithin [0,size)
  intX += (abs(intX) / size + 1) * size;
  intY += (abs(intY) / size + 1) * size;

  float v0 = pixels[(intX - 1)%size * size + intY%size];
  float v1 = pixels[(intX    )%size * size + intY%size];
  float v2 = pixels[(intX + 1)%size * size + intY%size];
  float v3 = pixels[(intX + 2)%size * size + intY%size];

  return cubicInterpolate(v0, v1, v2, v3, fracX);
}

// Interpolation of y
float GeneratedImage::bicubicInterpolate(float x, float y, const std::vector<float>& pixels) {
  int intX = (int)x;
  float fracX = x - intX;
  int intY = (int)y;
  float fracY = y - intY;

  float t0 = bicubicFirstDim(intX, intY - 1, fracX, pixels);
  float t1 = bicubicFirstDim(intX, intY,     fracX, pixels);
  float t2 = bicubicFirstDim(intX, intY + 1, fracX, pixels);
  float t3 = bicubicFirstDim(intX, intY + 2, fracX, pixels);

  return cubicInterpolate(t0, t1, t2, t3, fracY);
}

float GeneratedImage::getValueNormalizedCoord(float x, float y) const {
  if (x < 0)
    x = 0;
  else if (x >= 1)
    x = 1;

  if (y < 0)
    y = 0;
  else if (y >= 1)
    y = 1;

  return bicubicInterpolate(x*_size, y*_size, _pixels);
}
