#pragma once

#include "eventHandler.h"

class EventHandlerGlobalView : public EventHandler {
public:
  EventHandlerGlobalView(Game& game, SDL2pp::Window& window);

  bool handleEvent(const SDL_Event& event);
  void onGoingEvents(int msElapsed);

  void gainFocus();

private:
  void handleKeyPressed(const SDL_Event& event);

  bool _addSelect;
  float _scrollSpeed;

  glm::ivec4 _rectSelect;
};
