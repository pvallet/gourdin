#include "perlin.h"

#include <cstdlib>
#include <cmath>
#include <iostream>

Perlin::Perlin(int seed, int size) :
  _size(size),
  _octaves(5),
  _frequency(0.01),
  _persistence(0.5) {

  _frequency *= (float) 256 / (float) (_size - 1);

  srand(seed);

  _size = _size * _frequency * pow(2, _octaves) + 3; // Add an extra border for interpolation

  // We create a different array of random values, thus a different space for each octave
  std::vector<float> init(_size*_size);
  _randValues.resize(_octaves,init);

  for (size_t i = 0 ; i < _size ; i++) {
    for (size_t j = 0 ; j < _size ; j++) {
      for (size_t k = 0 ; k < _octaves ; k++) {
        _randValues[k][_size*i + j] = (float) rand() / (float) RAND_MAX;
      }
    }
  }
}

float Perlin::cubic_interpolate(float before_p0, float p0, float p1, float after_p1, float t) {
  float a3 = -0.5*before_p0 + 1.5*p0 - 1.5*p1 + 0.5*after_p1;
  float a2 = before_p0 - 2.5*p0 + 2*p1 - 0.5*after_p1;
  float a1 = -0.5*before_p0 + 0.5*p1;
  float a0 = p0;

  return (a3 * t*t*t) + (a2 * t*t) + (a1 * t) + a0;
}

// Interpolation of a line for a fixed y
float Perlin::smooth_noise_firstdim(int integer_x, int integer_y, float fractional_x, int octave) const {
  integer_x++; // Avoid requests for negative x & y
  integer_y++;

  float v0 = _randValues[octave][(integer_x - 1)*_size + integer_y];
  float v1 = _randValues[octave][(integer_x    )*_size + integer_y];
  float v2 = _randValues[octave][(integer_x + 1)*_size + integer_y];
  float v3 = _randValues[octave][(integer_x + 2)*_size + integer_y];

  return cubic_interpolate(v0, v1, v2, v3, fractional_x);
}

// Interpolation of the y
float Perlin::smooth_noise(float x, float y, int octave) const {
  int integer_x = (int)x;
  float fractional_x = x - integer_x;
  int integer_y = (int)y;
  float fractional_y = y - integer_y;

  float t0 = smooth_noise_firstdim(integer_x, integer_y - 1, fractional_x, octave);
  float t1 = smooth_noise_firstdim(integer_x, integer_y,     fractional_x, octave);
  float t2 = smooth_noise_firstdim(integer_x, integer_y + 1, fractional_x, octave);
  float t3 = smooth_noise_firstdim(integer_x, integer_y + 2, fractional_x, octave);

  return cubic_interpolate(t0, t1, t2, t3, fractional_y);
}


float Perlin::getValue(float x, float y) const {
  float r = 0.f;
  float f = _frequency;
  float amplitude = 1.f;

  for (size_t i = 0; i < _octaves; i++) {
    r += smooth_noise(x * f, y * f, i) * amplitude;
    amplitude *= _persistence;
    f *= 2;
  }

  float geo_lim = (1 - _persistence) / (1 - amplitude); // To keep the result < 1.f

  return r * geo_lim;
}
