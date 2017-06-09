#pragma once

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

class Engine;

class Interface {
public:
  Interface(sf::RenderWindow& window);
  void init();

  void renderMinimap(const Engine& engine) const;
  void renderTextTopLeft(const std::string& string) const;
  void renderTextTopRight(const std::string& string) const;
  void renderRectSelect() const;

  sf::Vector2f getMinimapClickCoord(float x, float y) const;
  void setRectSelect(sf::IntRect rect);

private:
  sf::RectangleShape _rectSelect;
  sf::Font _logFont;
  sf::Text _log;
  sf::Sprite _minimapSprite;
  sf::Texture _minimapTexture;

  sf::RenderWindow& _window;
};
