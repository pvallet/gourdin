#include "generatedImage.h"

#include <SDL_image.h>
#include <SDL2pp/SDL2pp.hh>

#include <algorithm>
#include <cassert>
#include <cmath>

GeneratedImage::GeneratedImage() :
  _size(0) {}

GeneratedImage::GeneratedImage(size_t size, float color) :
  _size(size),
  _pixels(size*size, color) {}

GeneratedImage::GeneratedImage(std::vector<float> pixels) {
  setPixels(pixels);
}

void GeneratedImage::setPixels(const std::vector<float>& pixels) {
  _size = sqrt(pixels.size());

  assert(pixels.size() == _size*_size);

  _pixels = pixels;
}

union ConvertFloat {
  float f;
  uint8_t uc[4];
};

bool GeneratedImage::loadFromFile(std::string filename) {
  try {
    SDL2pp::Surface img(filename);

    if (img.GetWidth() != img.GetHeight()) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error in GeneratedImage::loadFromFile: %s is not square.", filename.c_str());
      return false;
    }

    const uint8_t* imgPixels = (const uint8_t*) img.Get()->pixels;
    _size = img.GetHeight();
    _pixels.resize(_size*_size, 0);

    #pragma omp parallel for
    for (int i = 0; i < _size*_size; i++) {
      ConvertFloat convert = { 0.f };
      for (int j = 0; j < 4; j++) {
        convert.uc[j] = imgPixels[4*i + j];
      }
      _pixels[i] = convert.f;
    }

    return true;

  } catch (std::exception& e) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", e.what());
    return false;
  }
}

void GeneratedImage::saveToFile(std::string filename) const {
  std::vector<uint8_t> rgbPixels(4*_pixels.size());

  #pragma omp parallel for
  for (int i = 0; i < _pixels.size(); i++) {
    ConvertFloat convert;
    convert.f = _pixels[i];

    for (int j = 0; j < 4; j++) {
      rgbPixels[4*i + j] = convert.uc[j];
    }
  }

  // Little endian
  SDL2pp::Surface img(&rgbPixels[0], _size, _size, 32, 4*_size, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
	IMG_SavePNG(img.Get(), filename.c_str());
}

void GeneratedImage::invert() {
  #pragma omp parallel for
  for (int i = 0; i < _size*_size; i++) {
    _pixels[i] = 1 - _pixels[i];
  }
}

void GeneratedImage::normalize() {
  float max = *std::max_element(_pixels.begin(), _pixels.end());
  *this /= max;
}

void GeneratedImage::combine(const std::vector<float>& img, const std::vector<float>& mask) {
  assert(img.size() == _pixels.size());
  assert(mask.size() == _pixels.size());

  #pragma omp parallel for
  for (int i = 0; i < _pixels.size(); i++) {
    _pixels[i] = img[i] * mask[i] + _pixels[i] * (1-mask[i]);
  }
}

void GeneratedImage::applyConvolutionFilter(const std::vector<float>& filter) {
  size_t filterSize = sqrt(filter.size());

  assert(filterSize % 2 == 1);
  assert(filter.size() == filterSize*filterSize);

  std::vector<float> nPixels(_size*_size, 0);

  #pragma omp parallel for
  for (int i = 0; i < _size; i++) {
  for (int j = 0; j < _size; j++) {
    for (int k = 0; k < filterSize; k++) {
    for (int l = 0; l < filterSize; l++) {
      int curI = i - filterSize/2 + k;
      int curJ = j - filterSize/2 + l;

      if (curI < 0)
        curI += _size;
      else if (curI >= (int) _size)
        curI -= _size;

      if (curJ < 0)
        curJ += _size;
      else if (curJ >= (int) _size)
        curJ -= _size;

      nPixels[i*_size + j] += _pixels[curI*_size + curJ] * filter[k*filterSize + l];
    }
    }
  }
  }

  _pixels = nPixels;
}

void GeneratedImage::dilatation(float radius, std::function<bool(float)> belongsToExpandedRegion) {
  int dilSize = round(radius)+1;

  std::vector<float> dilMask(dilSize*dilSize, 0);

  // Generate dilatation mask according to the distances. Only one quarter of it as it is symmetrical
  #pragma omp parallel for
  for (int i = 0; i < (size_t) dilSize; i++) {
    for (int j = 0; j < (size_t) dilSize; j++) {
      if (sqrt(i*i + j*j) <= radius)
        dilMask[i*dilSize + j] = 1;
    }
  }

  std::vector<float> result = _pixels;
  #pragma omp parallel for
  for (int i = 0; i < (int) _size; i++) {
  for (int j = 0; j < (int) _size; j++) {

    if (belongsToExpandedRegion(_pixels[i*_size + j])) {
      for (int k = std::max(0, i-dilSize+1); k < std::min((int)_size, i+dilSize); k++) {
      for (int l = std::max(0, j-dilSize+1); l < std::min((int)_size, j+dilSize); l++) {

        if (dilMask[abs(k-i)*dilSize + abs(l-j)] == 1)
          result[k*_size + l] = _pixels[i*_size + j];
      }
      }
    }
  }
  }

  _pixels = result;
}

void GeneratedImage::smoothBlackDilatation(float radius) {
  int dilSize = round(radius)+1;

  std::vector<float> dilMask(dilSize*dilSize);

  // Generate dilatation mask according to the distances. Only one quarter of it as it is symmetrical
  #pragma omp parallel for
  for (int i = 0; i < (size_t) dilSize; i++) {
    for (int j = 0; j < (size_t) dilSize; j++) {
      dilMask[i*dilSize + j] = sqrt(i*i + j*j) / radius;
    }
  }

  #pragma omp parallel for
  for (int i = 0; i < (int) _size; i++) {
  for (int j = 0; j < (int) _size; j++) {
    // Mask is only applied to expand black regions
    if (_pixels[i*_size + j] == 0.f) {
      for (int k = std::max(0, i-dilSize+1); k < std::min((int)_size, i+dilSize); k++) {
      for (int l = std::max(0, j-dilSize+1); l < std::min((int)_size, j+dilSize); l++) {
        size_t currentIndex = k*_size + l;
        if (_pixels[currentIndex] == 0)
          continue;
        _pixels[currentIndex] = std::min(_pixels[currentIndex], dilMask[abs(k-i)*dilSize + abs(l-j)]);
      }
      }
    }
  }
  }
}

std::vector<float> GeneratedImage::generateBoxFilter(size_t size) {
  if (size % 2 == 0)
    size--;

  std::vector<float> filter(size*size, 0);

  #pragma omp parallel for
  for (int i = 0; i < size*size; i++) {
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
  #pragma omp parallel for
  for (int i = 0; i < size*size; i++) {
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

GeneratedImage& GeneratedImage::operator+=(const GeneratedImage& rhs) {
  assert(rhs._size == _size);
  #pragma omp parallel for
  for (int i = 0; i < _pixels.size(); i++) {
    _pixels[i] += rhs._pixels[i];
  }
  return *this;
}

GeneratedImage& GeneratedImage::operator-=(const GeneratedImage& rhs) {
  assert(rhs._size == _size);
  #pragma omp parallel for
  for (int i = 0; i < _pixels.size(); i++) {
    _pixels[i] -= rhs._pixels[i];
  }
  return *this;
}

GeneratedImage& GeneratedImage::operator*=(const GeneratedImage& rhs) {
  assert(rhs._size == _size);
  #pragma omp parallel for
  for (int i = 0; i < _pixels.size(); i++) {
    _pixels[i] *= rhs._pixels[i];
  }
  return *this;
}

GeneratedImage& GeneratedImage::operator/=(const GeneratedImage& rhs) {
  assert(rhs._size == _size);
  #pragma omp parallel for
  for (int i = 0; i < _pixels.size(); i++) {
    _pixels[i] /= rhs._pixels[i];
  }
  return *this;
}

GeneratedImage& GeneratedImage::operator+=(float rhs) {
  #pragma omp parallel for
  for (int i = 0; i < _pixels.size(); i++) {
    _pixels[i] += rhs;
  }
  return *this;
}

GeneratedImage& GeneratedImage::operator-=(float rhs) {
  #pragma omp parallel for
  for (int i = 0; i < _pixels.size(); i++) {
    _pixels[i] -= rhs;
  }
  return *this;
}

GeneratedImage& GeneratedImage::operator*=(float rhs) {
  #pragma omp parallel for
  for (int i = 0; i < _pixels.size(); i++) {
    _pixels[i] *= rhs;
  }
  return *this;
}

GeneratedImage& GeneratedImage::operator/=(float rhs) {
  #pragma omp parallel for
  for (int i = 0; i < _pixels.size(); i++) {
    _pixels[i] /= rhs;
  }
  return *this;
}
