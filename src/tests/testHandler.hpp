#pragma once

#include "controller.h"

class TestHandler {
public:
  TestHandler () {}

  void runTests(const Controller& controller);
  void clean();

private:
};
