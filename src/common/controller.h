#pragma once

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include "event_handler_game.h"
#include "event_handler_sandbox.h"
#include "game.h"
#include "interface.h"

/* Controls the main loop of the program
 */
class Controller {
public:
	Controller(sf::RenderWindow& window);

	void init();
	void run();

	const Game& getGame() const {return _game;}

private:
	void render() const;

	void benchmark();

	bool _running;
	sf::Time _elapsed;

	Interface _interface;
	Game _game;

	EventHandlerGame    _eHandlerGame;
	EventHandlerSandbox _eHandlerSandbox;
	EventHandlerType    _currentHandlerType;

	sf::RenderWindow& _window;
};
