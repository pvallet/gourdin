#pragma once

#include "event_handler.h"

class Controllable;

class EventHandlerGame : public EventHandler {
public:
  EventHandlerGame(Game& game, Interface& interface);

  bool handleEvent(const sf::Event& event, EventHandlerType& currentHandler);
  void onGoingEvents(const sf::Time& elapsed);

  void gainFocus();

  // Returns two solutions in the range [0,360)
  static std::pair<float, float> solveAcosXplusBsinXequalC(float a, float b, float c);

private:
  void handleKeyPressed(const sf::Event& event);
  void handleKeyReleased(const sf::Event& event);
  void handleCameraGodMode(const sf::Time& elapsed, float& theta, float& phi) const;
  void handleCameraPOVMode(const sf::Time& elapsed, float& theta, float& phi) const;

  const float _minScalarProductWithGround;

  bool _povCamera;

  float _oldTheta;
  float _oldPhi;

  bool _draggingCamera;

  sf::Clock _transferStart;
  sf::Vector2f _previousFocusedPos;

  Controllable* _focusedCharacter;
};
