#pragma once

#include <cassert>
#include <cstddef>
#include <functional>
#include <string>
#include <vector>

// Handles a square black and white image
class GeneratedImage {
public:
  GeneratedImage();
  GeneratedImage(size_t size, float color);
  GeneratedImage(std::vector<float> pixels);

  void setPixels(const std::vector<float>& pixels);
  // We store the 4 bytes of the floats in the RGBA channels
  bool loadFromFile(std::string filename);
  void saveToFile(std::string filename) const;

  void invert();
  void normalize();
  // For the edges, wraps the image around
  void applyConvolutionFilter(const std::vector<float>& filter);
  void combine(const std::vector<float>& img, const std::vector<float>& mask);
  // Morphologic dilatation of the region defined by the function belongsToExpandedRegion
  void dilatation(float radius, std::function<bool(float)> belongsToExpandedRegion);
  // Morphologic dilatation of the white region where grayness depends on the distance to the white region
  void smoothBlackDilatation(float radius);

  inline const std::vector<float>& getPixels() const {return _pixels;}
  float getValueNormalizedCoord(float x, float y) const;

  static float bicubicInterpolate(float x, float y, const std::vector<float>& pixels);

  // If the filter size is even, the generator makes it odd
  static std::vector<float> generateBoxFilter(size_t size);
  static std::vector<float> generateGaussianFilter(size_t size, float sigma);

private:
  static float cubicInterpolate(float before_p0, float p0, float p1, float after_p1, float t);
  static float bicubicFirstDim(int intX, int intY, float fracX, const std::vector<float>& pixels);

  size_t _size;

  std::vector<float> _pixels;

public:

  // Overloaded operators

  inline float& operator[](size_t index)       {assert(index < _size*_size); return _pixels[index];}
  inline float  operator[](size_t index) const {assert(index < _size*_size); return _pixels[index];}

  GeneratedImage& operator+=(const GeneratedImage& rhs);
  friend GeneratedImage operator+(GeneratedImage lhs, const GeneratedImage& rhs) {
    lhs += rhs; return lhs;
  }

  GeneratedImage& operator-=(const GeneratedImage& rhs);
  friend GeneratedImage operator-(GeneratedImage lhs, const GeneratedImage& rhs) {
    lhs -= rhs; return lhs;
  }

  GeneratedImage& operator*=(const GeneratedImage& rhs);
  friend GeneratedImage operator*(GeneratedImage lhs, const GeneratedImage& rhs) {
    lhs *= rhs; return lhs;
  }

  GeneratedImage& operator/=(const GeneratedImage& rhs);
  friend GeneratedImage operator/(GeneratedImage lhs, const GeneratedImage& rhs) {
    lhs /= rhs; return lhs;
  }

  // We define operators for simple values to avoid creating a new image

  GeneratedImage& operator+=(float rhs);
  friend GeneratedImage operator+(GeneratedImage lhs, float rhs) {
    lhs += rhs; return lhs;
  }

  GeneratedImage& operator-=(float rhs);
  friend GeneratedImage operator-(GeneratedImage lhs, float rhs) {
    lhs -= rhs; return lhs;
  }

  GeneratedImage& operator*=(float rhs);
  friend GeneratedImage operator*(GeneratedImage lhs, float rhs) {
    lhs *= rhs; return lhs;
  }

  GeneratedImage& operator/=(float rhs);
  friend GeneratedImage operator/(GeneratedImage lhs, float rhs) {
    lhs /= rhs; return lhs;
  }
};
