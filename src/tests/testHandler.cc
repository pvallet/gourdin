#include "testHandler.hpp"

#include <cstdio>

void TestHandler::runTests(const Controller& controller) {
  const Game& game = controller.getGame();
  game._reliefGenerator.saveToImage("256.png", 256);
  game._reliefGenerator.saveToImage("512.png", 512);
  game._reliefGenerator.saveToImage("1024.png", 1024);

  game._contentGenerator.saveToImage("contents.png");
}

void TestHandler::clean() {
  remove("256.png");
  remove("512.png");
  remove("1024.png");
  remove("contents.png");
}
