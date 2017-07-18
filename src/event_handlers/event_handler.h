#pragma once

#include <SDL.h>
#include <glm/glm.hpp>

#include <map>

#define DOUBLECLICK_MS 200

enum EventHandlerType {HDLR_GAME, HDLR_SANDBOX};

class EventHandler {
public:
  EventHandler() {}

  // Returns whether the engine will stop or not
  virtual bool handleEvent(const SDL_Event& event, EventHandlerType& currentHandler);
  virtual void onGoingEvents(int msElapsed) = 0;

  virtual bool gainFocus() = 0;

  // Returns two solutions in the range [0,360) in ascending order
  static std::pair<float, float> solveAcosXplusBsinXequalC(float a, float b, float c);
  // Takes two angles in [0,360)
  // Returns the distance between two angles in degrees mod 360
  static float absDistBetweenAngles(float a, float b);

  static void makeThetaFitInAllowedZone(float& theta, const glm::vec3& normal, float minDotProduct);

protected:
  glm::ivec2 _beginDragLeft;
};
