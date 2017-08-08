#pragma once

#include <SDL2pp/SDL2pp.hh>
#include "opengl.h"

#include "eventHandlerLockedView.h"
#include "eventHandlerGlobalView.h"
#include "engine.h"
#include "game.h"

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
	Game _game;

	EventHandlerLockedView _eventHandlerLockedView;
	EventHandlerGlobalView _eventHandlerGlobalView;

	SDL2pp::Window& _window;
};
