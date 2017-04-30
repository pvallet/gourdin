#pragma once

#include <SFML/System.hpp>

#include <vector>

class GeneratedImage {
public:
  GeneratedImage(std::vector<float> blackNwhitePixels);

  inline std::vector<sf::Uint8> getPixels() const {return _pixels;}

private:
  size_t _size;

  std::vector<sf::Uint8> _pixels;
};
