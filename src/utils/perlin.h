#pragma once

#include <cstddef>
#include <vector>

class Perlin {
public:
  Perlin(int seed, int size);
  virtual ~Perlin() {}

  float getValue(float x, float y) const;

  static float cubic_interpolate(float before_p0, float p0, float p1, float after_p1, float t);

private:
  size_t _size;
  size_t _octaves;
  float _frequency;
  float _persistence;

  std::vector<std::vector<float> > _randValues;

  float smooth_noise_firstdim(int integer_x, int integer_y, float fractional_x, int octave) const;
  float smooth_noise(float x, float y, int octave) const;
};
