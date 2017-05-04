#pragma once

#include <cstddef>
#include <string>
#include <vector>

// Handles a square black and white image
class GeneratedImage {
public:
  GeneratedImage();
  GeneratedImage(std::vector<float> pixels);

  void setPixels(const std::vector<float>& pixels);
  // We store the 4 bytes of the floats in the RGBA channels
  bool loadFromFile(std::string filename);
  void saveToFile(std::string filename) const;

  void invert();
  void multiply(const std::vector<float>& img);
  void addAndNormalize(const std::vector<float>& img, float weightAdding);
  // For the edges, wraps the image around
  void applyConvolutionFilter(const std::vector<float>& filter);
  // Morphologic dilatation of the black regions, with grayness depending on the distance to the black region
  void smoothDilatation(float radius);

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
};
