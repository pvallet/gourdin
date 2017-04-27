#include "testHandler.hpp"

TestHandler::TestHandler(const Controller& controller) :
  _controller(controller),
  _game(controller.getGame()) {}

void TestHandler::runTests() {
  _game._reliefGenerator.saveToImage("256.png", 256);
  _game._reliefGenerator.saveToImage("512.png", 512);
  _game._reliefGenerator.saveToImage("1024.png", 1024);

  _game._contentGenerator.saveToImage("contents.png");
}
