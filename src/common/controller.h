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
	void benchmark(size_t range);

	void render() const;

private:
	void renderLifeBars() const;
	void renderMinimap() const;
	void renderLog() const;
	void renderInfo() const;


	void handleClickSandbox(sf::Event event);
	void handleKeyPressedSandbox(sf::Event event);
	void handleEventSandbox(sf::Event event);
	void moveCameraSandbox() const;

	void handleEventGame(sf::Event event);
	void moveCameraGame() const;

	bool _addSelect;
	bool _selecting;
	sf::RectangleShape _rectSelect;
	sf::Font _logFont;
	sf::Text _log;
	sf::Sprite _minimapSprite;
	sf::Texture _minimapTexture;

	bool _running;
	bool _inGameMode;
	sf::Time _elapsed;
	sf::RenderWindow& _window;

	Game _game;
};
