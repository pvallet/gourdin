// Code inspired by tutorial:
// http://cochoy-jeremy.developpez.com/tutoriels/2d/introduction-bruit-perlin/

#pragma once

#include <cstddef>
#include <string>
#include <vector>

class Perlin {
public:
  Perlin(size_t octaves, float frequency, float persistence, size_t size = 512, int seed = 0);

  // x and y are between 0 and _size-1
  float getValue(float x, float y) const;

  // x and y are between 0 and 1
  inline float getValueNormalizedCoord(float x, float y) const {
    return getValue(x >= 1 ? _size-1 : x*_size, y >= 1 ? _size-1 : y*_size);}

  void shuffle();


  static float cubic_interpolate(float before_p0, float p0, float p1, float after_p1, float t);

  std::vector<float> getPixels() const;
  inline size_t getSize() const {return _size;}

private:
  size_t _size;
  size_t _sizeRandArray;
  size_t _octaves;
  float _frequency;
  float _persistence;

  std::vector<std::vector<float> > _randValues;

  float smooth_noise_firstdim(int integer_x, int integer_y, float fractional_x, int octave) const;
  float smooth_noise(float x, float y, int octave) const;
};
