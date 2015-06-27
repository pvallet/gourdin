#pragma once

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include "camera.h"
#include "game.h"
#include "map.h"


class Controller {
public:
	Controller(sf::RenderWindow* _window);
	
	void init();
	void run();

	void render();

private:
	bool addSelect;
	bool selecting;
	sf::RectangleShape rectSelect;
	sf::Font font;
	sf::Text fpsCounter;
	sf::Sprite minimap;
	Map map;

	bool running;
	sf::Clock frameClock;
	sf::Time elapsed;
	sf::RenderWindow* window;

	Camera camera;
	Game game;
};

