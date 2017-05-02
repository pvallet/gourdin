#pragma once

#include <cstddef>
#include <vector>

// Handles a square black and white image
class GeneratedImage {
public:
  GeneratedImage();
  GeneratedImage(std::vector<float> pixels);

  void invert();
  // For the edges, wraps the image around
  void applyConvolutionFilter(const std::vector<float>& filter);
  void multiply(const std::vector<float>& img);

  inline const std::vector<float>& getPixels() const {return _pixels;}
  float getValueNormalizedCoord(float x, float y) const;

  // If the filter size is even, the generator makes it odd
  static std::vector<float> generateBoxFilter(size_t size);
  static std::vector<float> generateGaussianFilter(size_t size, float sigma);

private:
  size_t _size;

  std::vector<float> _pixels;
};
