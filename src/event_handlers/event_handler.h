#pragma once

#include <SDL.h>
#include <glm/glm.hpp>

#include "game.h"

#include <map>

#define DEFAULT_OUTSIDE_WINDOW_COORD glm::ivec2(-1,-1)

#define MAX_DIST_FOR_CLICK 40.f

enum EventHandlerType {HDLR_GAME, HDLR_SANDBOX};

class EventHandler {
public:
  EventHandler(Game& game);

  // Returns whether the engine will stop or not
  virtual bool handleEvent(const SDL_Event& event, EventHandlerType& currentHandler);
  virtual void onGoingEvents(int msElapsed);

  virtual bool gainFocus() = 0;

  // Returns two solutions in the range [0,360) in ascending order
  static std::pair<float, float> solveAcosXplusBsinXequalC(float a, float b, float c);
  // Takes two angles in [0,360)
  // Returns the distance between two angles in degrees mod 360
  static float absDistBetweenAngles(float a, float b);

  static void makeThetaFitInAllowedZone(float& theta, const glm::vec3& normal, float minDotProduct);

protected:
  inline size_t getNbFingers() const {return _nbFingers;}
  glm::ivec2 _beginDragLeft;
  glm::ivec2 _beginDragTouch;

  static Uint32 SDL_USER_FINGER_CLICK;
  static Uint32 SDL_USER_FINGER_DOUBLE_CLICK;

private:
  static size_t _nbFingers;

  glm::ivec2 _pendingClick;
  Clock _doubleClickBegin;

  Game& _game;
};
