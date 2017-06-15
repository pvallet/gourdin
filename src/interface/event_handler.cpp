#include "event_handler.h"
#include "camera.h"
#include "clock.h"

bool EventHandler::handleEvent(const sf::Event& event, EventHandlerType& currentHandler) {
  bool running = true;

  if (event.type == sf::Event::Closed)
    running = false;

  else if (event.type == sf::Event::Resized) {
    Camera& cam = Camera::getInstance();
    cam.resize(event.size.width, event.size.height);
  }

  else if (event.type == sf::Event::KeyPressed) {
    switch(event.key.code) {
      case sf::Keyboard::Escape:
        running = false;
        break;

      case sf::Keyboard::M:
        if (currentHandler == HDLR_GAME)
          currentHandler = HDLR_SANDBOX;
        else
          currentHandler = HDLR_GAME;
        break;

      case sf::Keyboard::P:
        if (Clock::isGlobalTimerPaused())
          Clock::resumeGlobalTimer();
        else
          Clock::pauseGlobalTimer();
        break;
    }
  }

  else if (event.type == sf::Event::MouseButtonPressed) {
    if (event.mouseButton.button == sf::Mouse::Left)
      _beginDragLeft = glm::ivec2(event.mouseButton.x, event.mouseButton.y);
  }

  else if (event.type == sf::Event::MouseButtonReleased) {
    if (event.mouseButton.button == sf::Mouse::Left)
      _beginDragLeft = glm::ivec2(0,0);
  }

  return running;
}

std::pair<float, float> EventHandler::solveAcosXplusBsinXequalC(float a, float b, float c) {
  // The code is a transcription of Wolfram Alpha solution
  std::pair<float,float> res(0,0);

  if (a == -c) {
    res.first = 180;
    res.second = 180;
  }

  else {
    float underRoot = a*a + b*b - c*c;
    if (underRoot >= 0) {
      res.first  = 2 * atan((b - sqrt(underRoot)) / (a + c)) / RAD;
      res.second = 2 * atan((b + sqrt(underRoot)) / (a + c)) / RAD;

      // Results are in range (-180, 180], make them in range [0,360)
      if (res.first < 0)
        res.first += 360;
      if (res.second < 0)
        res.second += 360;
    }

    // Ensure the ascending order
    if (res.second < res.first)
      std::swap(res.first,res.second);

    }
  return res;
}

float EventHandler::absDistBetweenAngles(float a, float b) {
  return std::min(std::abs(a-b),std::abs(std::abs(a-b)-360));
}

void EventHandler::makeThetaFitInAllowedZone(float& theta, const glm::vec3& normal, float minDotProduct) {
  if (glm::dot(normal, ut::carthesian(1, theta, 90)) < minDotProduct) {
    // New theta with given phi
    std::pair<float,float> thetasLim = solveAcosXplusBsinXequalC(
      normal.x, normal.y, minDotProduct);

    std::pair<float,float> distsToThetasLim;
    distsToThetasLim.first  = absDistBetweenAngles(theta, thetasLim.first);
    distsToThetasLim.second = absDistBetweenAngles(theta, thetasLim.second);

    if (distsToThetasLim.first < distsToThetasLim.second)
      theta = thetasLim.first;
    else
      theta = thetasLim.second;
  }
}
