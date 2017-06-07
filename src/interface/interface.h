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
  void renderLog(sf::Time elapsed) const;
  void renderInfo(bool inGameMode) const;
  void renderRectSelect() const;

  sf::Vector2f getMinimapClickCoord(float x, float y) const;
  void setRectSelect(sf::IntRect rect);

  inline void switchLog() {_displayLog = !_displayLog;}
  inline void setPovCamera(bool povCamera) {_povCamera = povCamera;}
  inline void setScrollSpeedToSlow(bool scrollSpeedSlow) {_scrollSpeedSlow = scrollSpeedSlow;}

private:

  bool _displayLog;
  bool _povCamera;
  bool _scrollSpeedSlow;
  sf::RectangleShape _rectSelect;
  sf::Font _logFont;
  sf::Text _log;
  sf::Sprite _minimapSprite;
  sf::Texture _minimapTexture;

  sf::RenderWindow& _window;
};
