#pragma once

#include "chunk.h"

class Perlin {
public:
  Perlin(int seed, int size);
  ~Perlin();
  double getValue(double x, double y) const;

  static double cubic_interpolate(double before_p0, double p0, double p1, double after_p1, double t);

private:
  int _size;
  int _octaves;
  double _frequency;
  double _persistence;

  double** _randValues;

  double smooth_noise_firstdim(int integer_x, int integer_y, double fractional_x, int octave) const;
  double smooth_noise(double x, double y, int octave) const;
};
