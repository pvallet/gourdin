#pragma once

#include <cstddef>
#include <stddef.h> // size_t
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

  std::vector<float> getPixels() const;
  inline size_t getSize() const {return _size;}

private:
  size_t _size;
  size_t _sizeRandArray;
  size_t _octaves;
  float _frequency;
  float _persistence;

  std::vector<std::vector<float> > _randValues;
};
