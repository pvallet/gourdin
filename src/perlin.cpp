#include "perlin.h"

#include <cstdlib>
#include <cmath>
#include <iostream>

/*double rand_noise(int t) // Random double between -1. and 1.
{
    t = (t<<13) ^ t;
    t = (t * (t * t * 15731 + 789221) + 1376312589);
    return 1.0 - (t & 0x7fffffff) / 1073741824.0;
}

double noise(int x, int y) { 
    int tmp = (rand_noise(x) + 1.) / 2 * 850000;
    return (rand_noise(tmp + y) + 1.) / 2;
}*/

Perlin::Perlin(int seed, int _size) : 
    size(_size),
    octaves(5), 
    frequency(0.01), 
    persistence(0.5) {

    frequency *= (float) 256 / (float) (_size - 1);

    srand(seed);

    size = size * frequency * pow(2, octaves) + 3; // Add an extra border for interpolation

    randValues = new double*[octaves]; // We create a different array of random values, thus a different space for each octave

    for (int i = 0 ; i < octaves ; i++) {
        randValues[i] = new double[size*size];
    }

    for (int i = 0 ; i < size ; i++) {
        for (int j = 0 ; j < size ; j++) {
            for (int k = 0 ; k < octaves ; k++) {
                randValues[k][size*i + j] = (double) rand() / (double) RAND_MAX;
            }
        }
    }
}

Perlin::~Perlin() {
    for (int i = 0 ; i < octaves ; i++) {
        delete[] randValues[i];
    }

    delete[] randValues;
}

double Perlin::cubic_interpolate(double before_p0, double p0, double p1, double after_p1, double t) {
    double a3 = -0.5*before_p0 + 1.5*p0 - 1.5*p1 + 0.5*after_p1;
    double a2 = before_p0 - 2.5*p0 + 2*p1 - 0.5*after_p1;
    double a1 = -0.5*before_p0 + 0.5*p1;
    double a0 = p0;

    return (a3 * t*t*t) + (a2 * t*t) + (a1 * t) + a0;
}

// Interpolation of a line for a fixed y
double Perlin::smooth_noise_firstdim(int integer_x, int integer_y, double fractional_x, int octave) const {
    integer_x++; // Avoid requests for negative x & y
    integer_y++;

    double v0 = randValues[octave][(integer_x - 1)*size + integer_y];
    double v1 = randValues[octave][(integer_x    )*size + integer_y];
    double v2 = randValues[octave][(integer_x + 1)*size + integer_y];
    double v3 = randValues[octave][(integer_x + 2)*size + integer_y];

    return cubic_interpolate(v0, v1, v2, v3, fractional_x);
}

// Interpolation of the y
double Perlin::smooth_noise(double x, double y, int octave) const {
    int integer_x = (int)x;
    double fractional_x = x - integer_x;
    int integer_y = (int)y;
    double fractional_y = y - integer_y;

    double t0 = smooth_noise_firstdim(integer_x, integer_y - 1, fractional_x, octave);
    double t1 = smooth_noise_firstdim(integer_x, integer_y,     fractional_x, octave);
    double t2 = smooth_noise_firstdim(integer_x, integer_y + 1, fractional_x, octave);
    double t3 = smooth_noise_firstdim(integer_x, integer_y + 2, fractional_x, octave);

    return cubic_interpolate(t0, t1, t2, t3, fractional_y);
}


double Perlin::getValue(double x, double y) const {
    double r = 0.;
    double f = frequency;
    double amplitude = 1.;

    for(int i = 0; i < octaves; i++) {
        r += smooth_noise(x * f, y * f, i) * amplitude;
        amplitude *= persistence;
        f *= 2;
    }

    double geo_lim = (1 - persistence) / (1 - amplitude); // To keep the result < 1.

    return r * geo_lim;
}

