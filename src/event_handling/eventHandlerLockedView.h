#pragma once

#include "eventHandler.h"

class EventHandlerLockedView : public EventHandler {
public:
  EventHandlerLockedView(Game& game);

  bool handleEvent(const SDL_Event& event);
  void onGoingEvents(int msElapsed);

  void gainFocus();

private:
  void handleKeyPressed(const SDL_Event& event);
  void handleKeyReleased(const SDL_Event& event);
  void handleCamBoundsGodMode(float& theta) const;
  void handleCamBoundsPOVMode(float& theta, float& phi) const;
  // The initial camera orientations are chosen according to the terrain normal
  void resetCamera(bool pov, int msTransferDuration = 0);
  static float getPhiLimForGivenTheta(float theta, glm::vec3 normal, float maxDotProduct);
  static bool firstIsOnPositiveSideOfSecond(float first, float second);

  // Scalar product between terrain normal and (1,cam.theta,90)
  const float _maxScalarProductWithGroundPOV;
  const float _minScalarProductWithGroundGod;

  float _oldTheta;
  float _oldPhi;

  glm::ivec2 _beginDrag;

  Chronometer _characterTransferTimer;
  glm::vec2 _previousFocusedPos;

  Chronometer _camTransferTimer;
  glm::vec4 _previousCameraParams;
  glm::vec4 _nextCameraParams;
  float _thetaInPreviousView;
  float _phiInPreviousView;
};
