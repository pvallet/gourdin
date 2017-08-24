#pragma once

#include "eventHandler.h"

// When not on POV, the camera can be locked in two positions, with an angle either
// positive or negative to the angle of steepest slope of the current pointed position
enum PreviousCameraLock {NO_LOCK, TRIGO_POS, TRIGO_NEG};

class EventHandlerLockedView : public EventHandler {
public:
  EventHandlerLockedView(Game& game);

  bool handleEvent(const SDL_Event& event);
  void onGoingEvents(int msElapsed);

  void gainFocus();

private:
  void handleKeyPressed(const SDL_Event& event);
  void handleKeyReleased(const SDL_Event& event);
  void handleCamBoundsGodMode(float& theta);
  void handleCamBoundsPOVMode(float& theta, float& phi, bool slideCameraWhenGoingDownwards) const;
  // The initial camera orientations are chosen according to the terrain normal
  void resetCamera(bool pov, int msTransferDuration = 0);
  static float getPhiLimForGivenTheta(float theta, glm::vec3 normal, float maxDotProduct);

  // Scalar product between terrain normal and (1,cam.theta,90)
  const float _maxScalarProductWithGroundPOV;
  const float _minScalarProductWithGroundGod;

  float _previousTheta;
  float _previousPhi;
  float _initialDragTheta;
  float _initialDragPhi;

  PreviousCameraLock _previousCameraLock;

  glm::ivec2 _beginDrag;

  Chronometer _characterTransferTimer;
  glm::vec2 _previousFocusedPos;

  Chronometer _camTransferTimer;
  glm::vec4 _previousCameraParams;
  glm::vec4 _nextCameraParams;
  float _thetaInPreviousView;
  float _phiInPreviousView;
};
