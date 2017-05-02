#pragma once

#include "controller.h"

class TestHandler {
public:
  TestHandler () {}

  void runTests(const Controller& controller) const;
  void clean() const;

private:
  // The texture must be square of size x, pixels must be of size 4*x*x (rgba)
  void saveToImage(const std::vector<sf::Uint8>& pixels, std::string filename) const;
  // generates a black square on a white background
  std::vector<float> generateTestSquare(size_t size) const;
  std::vector<float> generateTestCircle(size_t size) const;

  void ContentGeneratorDisplayForestsMask(const ContentGenerator& contentGenerator, std::string savename) const;
  void PerlinSaveToImage(const Perlin& perlin, std::string savename) const;

  void displayGameGeneratedComponents(const Game& game) const;
  void testImageHandling() const;
};
