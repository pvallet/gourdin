#pragma once

#include "eventHandler.h"

class EventHandlerLockedView : public EventHandler {
public:
  EventHandlerLockedView(Game& game);

  bool handleEvent(const SDL_Event& event);
  void onGoingEvents(int msElapsed);

  inline void gainFocus() {resetCamera(false);}

private:
  void handleKeyPressed(const SDL_Event& event);
  void handleKeyReleased(const SDL_Event& event);
  void handleCamBoundsGodMode(float& theta) const;
  void handleCamBoundsPOVMode(float& theta, float& phi) const;
  // The initial camera orientations are chosen according to the terrain normal
  void resetCamera(bool pov);
  static float getPhiLimForGivenTheta(float theta, glm::vec3 normal, float maxDotProduct);
  static bool firstIsOnPositiveSideOfSecond(float first, float second);

  // Scalar product between terrain normal and (1,cam.theta,90)
  const float _maxScalarProductWithGroundPOV;
  const float _minScalarProductWithGroundGod;

  float _oldTheta;
  float _oldPhi;

  bool _draggingCamera;
  glm::ivec2 _beginDrag;

  Clock _transferStart;
  glm::vec2 _previousFocusedPos;
};
