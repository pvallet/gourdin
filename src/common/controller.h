#pragma once

#include <SDL2pp/SDL2pp.hh>
#include "opengl.h"

#include "android_event_handler_sandbox.h"
#include "event_handler_game.h"
#include "event_handler_sandbox.h"
#include "engine.h"
#include "gameGame.h"
#include "gameSandbox.h"

#ifndef NDEBUG
	class TestHandler;
#endif

/* Controls the main loop of the program
 */
class Controller {
public:
	Controller(SDL2pp::Window& window);

	void init();
	void run();

#ifndef NDEBUG
	friend TestHandler;
#endif

private:
	void benchmark();

	bool _running;
	int _msElapsed;

	Engine _engine;
	GameGame _gameGame;
	GameSandbox _gameSandbox;

	EventHandlerType    _currentHandlerType;
	EventHandlerGame    _eHandlerGame;

#ifdef __ANDROID__
	AndroidEventHandlerSandbox _eHandlerSandbox;
#else
	EventHandlerSandbox _eHandlerSandbox;
#endif

	SDL2pp::Window& _window;
};
