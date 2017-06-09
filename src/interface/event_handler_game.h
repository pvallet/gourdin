#pragma once

#include "event_handler.h"
#include "gameGame.h"

class EventHandlerGame : public EventHandler {
public:
  EventHandlerGame(GameGame& game);

  bool handleEvent(const sf::Event& event, EventHandlerType& currentHandler);
  void onGoingEvents(const sf::Time& elapsed);

  bool gainFocus();

  // Returns two solutions in the range [0,360) in ascending order
  static std::pair<float, float> solveAcosXplusBsinXequalC(float a, float b, float c);
  // Takes two angles in [0,360)
  // Returns the distance between two angles in degrees mod 360
  static float absDistBetweenAngles(float a, float b);

private:
  void handleKeyPressed(const sf::Event& event);
  void handleKeyReleased(const sf::Event& event);
  void handleCamBoundsGodMode(float& theta) const;
  void handleCamBoundsPOVMode(float& theta, float& phi) const;
  // The initial camera orientations are chosen according to the terrain normal
  void resetCamera(bool pov);
  static float getPhiLimForGivenTheta(float theta, sf::Vector3f normal, float maxDotProduct);
  static bool firstIsOnPositiveSideOfSecond(float first, float second);

  // Scalar product between terrain normal and (1,cam.theta,90)
  const float _maxScalarProductWithGroundPOV;
  const float _minScalarProductWithGroundGod;

  float _oldTheta;
  float _oldPhi;

  bool _draggingCamera;

  sf::Clock _transferStart;
  sf::Vector2f _previousFocusedPos;

  GameGame& _game;
};
