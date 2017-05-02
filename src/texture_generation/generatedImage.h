#pragma once

#include <SFML/System.hpp>

#include <vector>

class GeneratedImage {
public:
  GeneratedImage(std::vector<float> blackNwhitePixels);

  void invert();
  // For the edges, wraps the image around
  void applyConvolutionFilter(std::vector<float> filter);

  inline std::vector<sf::Uint8> getPixels() const {return _pixels;}

  // If the filter size is even, the generator makes it odd
  static std::vector<float> generateBoxFilter(size_t size);
  static std::vector<float> generateGaussianFilter(size_t size, float sigma);

private:
  size_t _size;

  std::vector<sf::Uint8> _pixels;
};
