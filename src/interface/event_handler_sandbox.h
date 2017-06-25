#pragma once

#include "event_handler.h"
#include "gameSandbox.h"

class EventHandlerSandbox : public EventHandler {
public:
  EventHandlerSandbox(GameSandbox& game);

  bool handleEvent(const SDL_Event& event, EventHandlerType& currentHandler);
  void onGoingEvents(int msElapsed);

  bool gainFocus();

private:
  void handleClick(const SDL_Event& event);
  void handleKeyPressed(const SDL_Event& event);

  bool _addSelect;
  float _scrollSpeed;

  glm::ivec4 _rectSelect;

  GameSandbox& _game;
};
