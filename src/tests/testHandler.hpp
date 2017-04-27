#pragma once

#include "controller.h"

class TestHandler {
public:
  TestHandler (const Controller& controller);

  void runTests();

private:
  const Controller& _controller;
  const Game& _game;
};
