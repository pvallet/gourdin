#pragma once

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include "event_handler_game.h"
#include "event_handler_sandbox.h"
#include "engine.h"
#include "interface.h"
#include "game.h"

#ifdef __APPLE__
#define CORE_PROFILE
#endif

#ifndef NDEBUG
	class TestHandler;
#endif

/* Controls the main loop of the program
 */
class Controller {
public:
	Controller(sf::RenderWindow& window);

	void init();
	void run();

#ifndef NDEBUG
	friend TestHandler;
#endif

private:
	void render() const;

	void benchmark();

	bool _running;
	sf::Time _elapsed;

	Interface _interface;
	Engine _engine;

	EventHandlerGame    _eHandlerGame;
	EventHandlerSandbox _eHandlerSandbox;
	EventHandlerType    _currentHandlerType;

	sf::RenderWindow& _window;
};
