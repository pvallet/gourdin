#pragma once

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include "camera.h"
#include "game.h"
#include "map.h"


class Controller {
public:
	Controller(sf::RenderWindow* window);

	void init();
	void run();

	void render();

private:
	bool _addSelect;
	bool _selecting;
	sf::RectangleShape _rectSelect;
	sf::Font _font;
	sf::Text _fpsCounter;
	sf::Text _posDisplay;
	sf::Sprite _minimap;
	Map _map;

	bool _running;
	sf::Clock _frameClock;
	sf::Time _elapsed;
	sf::RenderWindow* _window;

	Camera _camera;
	Game _game;
};
