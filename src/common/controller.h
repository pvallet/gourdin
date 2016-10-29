#pragma once

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include "game.h"

class Controller {
public:
	Controller(sf::RenderWindow& window);

	void init();
	void run();

	void render();

private:
	void renderLifeBars();
	void renderMinimap();
	void renderLog();

	bool _addSelect;
	bool _selecting;
	sf::RectangleShape _rectSelect;
	sf::Font _font;
	sf::Text _fpsCounter;
	sf::Text _posDisplay;
	sf::Sprite _minimapSprite;
	sf::Texture _minimapTexture;

	bool _running;
	sf::Clock _frameClock;
	sf::Time _elapsed;
	sf::RenderWindow& _window;

	Game _game;
};
