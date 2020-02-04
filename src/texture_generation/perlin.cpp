#include "perlin.h"

#include <cstdlib>
#include <cmath>

#include "generatedImage.h"

Perlin::Perlin(size_t octaves, float frequency, float persistence, size_t size, int seed) :
  _size(size),
  _octaves(octaves),
  _frequency(frequency),
  _persistence(persistence) {

  srand(seed);

  _sizeRandArray = _size * _frequency * pow(2, _octaves-1) + 4; // Add an extra border for interpolation

  // We create a different array of random values, thus a different space for each octave
  std::vector<float> init(_sizeRandArray*_sizeRandArray);
  _randValues.resize(_octaves,init);

  shuffle();
}

float Perlin::getValue(float x, float y) const {
  float r = 0.f;
  float f = _frequency;
  float amplitude = 1.f;

  for (int i = 0; i < _octaves; i++) {
    // r += smooth_noise(x * f, y * f, i) * amplitude;
    r += GeneratedImage::bicubicInterpolate(x * f, y * f, _randValues[i]) * amplitude;
    amplitude *= _persistence;
    f *= 2;
  }

  // normalization by the sum of max amplitudes (geometric sum)
  float geo_lim = (1 - _persistence) / (1 - amplitude*_persistence);

  float res = r*geo_lim;

  // Cubic interpolation can cause results to be out of range
  if (res > 1)
    res = 1;
  else if (res < 0)
    res = 0;

  return res;
}

std::vector<float> Perlin::getPixels() const {
  std::vector<float> img(_size*_size);

  #pragma omp parallel for
  for (int i = 0; i < _size; i++) {
    for (int j = 0; j < _size; j++) {
      img[i*_size + j] = getValue(i,j);
    }
  }

  return img;
}

void Perlin::shuffle() {
  for (int i = 0; i < _octaves; i++) {
    for (int j = 0; j < _sizeRandArray*_sizeRandArray; j++) {
      _randValues[i][j] = (float) rand() / (float) RAND_MAX;
    }
  }
}
