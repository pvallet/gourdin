#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SFML/OpenGL.hpp>

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
	Controller(SDL_Window* window);

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

	EventHandlerGame    _eHandlerGame;
	EventHandlerSandbox _eHandlerSandbox;
	EventHandlerType    _currentHandlerType;

	SDL_Window* _window;
};
