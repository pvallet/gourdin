#pragma once

#include "controller.h"

class TestHandler {
public:
  TestHandler () {}

  void runTests(const Controller& controller);
  void clean();

private:
  // The texture must be square of size x, pixels must be of size 4*x*x (rgba)
  void saveToImage(const std::vector<sf::Uint8>& pixels, std::string filename);

  void ContentGeneratorDisplayForestsMask(const ContentGenerator& contentGenerator, std::string savename);
  void PerlinSaveToImage(const Perlin& perlin, std::string savename, size_t size);
};
