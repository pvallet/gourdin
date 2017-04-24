#pragma once

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

class Game;

class Interface {
public:
  Interface(sf::RenderWindow& window);
  void init();

  void renderMinimap(const Game& game) const;
  void renderLog(sf::Time elapsed) const;
  void renderInfo(bool inGameMode) const;
  void renderRectSelect() const;

  sf::Vector2f getMinimapClickCoord(float x, float y) const;
  void setRectSelect(sf::IntRect rect);

  inline void switchLog() {_displayLog = !_displayLog;}
  inline void setPovCamera(bool povCamera) {_povCamera = povCamera;}

private:

  bool _displayLog;
  bool _povCamera;
  sf::RectangleShape _rectSelect;
  sf::Font _logFont;
  sf::Text _log;
  sf::Sprite _minimapSprite;
  sf::Texture _minimapTexture;

  sf::RenderWindow& _window;
};
