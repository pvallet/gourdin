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

float GeneratedImage::getValueNormalizedCoord(float x, float y) const {
  int intX = x * _size;
  int intY = y * _size;

  if (intX < 0)
    intX = 0;
  else if (intX >= _size)
    intX = _size-1;

  if (intY < 0)
    intY = 0;
  else if (intY >= _size)
    intY = _size-1;

  return _pixels[intX*_size + intY];
}
