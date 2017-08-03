#pragma once

#include "event_handler.h"
#include "gameSandbox.h"

class AndroidEventHandlerSandbox : public EventHandler {
public:
  AndroidEventHandlerSandbox(GameSandbox& game);

  bool handleEvent(const SDL_Event& event, EventHandlerType& currentHandler);

  bool gainFocus();

  static int HandleAppEvents(void *userdata, SDL_Event *event);
};
