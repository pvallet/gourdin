#pragma once

#include "controller.h"

class TestHandler {
public:
  TestHandler(const sf::Clock& beginningOfProg);

  void runTests(const Controller& controller) const;
  void clean() const;

private:
  // The texture must be square of size x, pixels must be of size 4*x*x (rgba)
  void saveToImage(const std::vector<sf::Uint8>& pixels, std::string filename) const;
  void saveToImage(const std::vector<float>& pixels, std::string filename) const;
  // generates a black square on a white background
  static std::vector<float> generateTestSquare(size_t size);
  static std::vector<float> generateTestCircle(size_t size);

  void ContentGeneratorDisplayForestsMask(const ContentGenerator& contentGenerator, std::string savename) const;

  void displayGameGeneratedComponents(const Game& game) const;
  void testVecUtils() const;
  void testPerlin() const;
  void testGeneratedImage() const;
  void testEventHandlerGame(const EventHandlerGame& eHandlerGame) const;

  const sf::Clock& _beginningOfProg;
};
