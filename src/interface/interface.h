#pragma once

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <set>

#include "engine.h"

class Interface {
public:
  Interface(sf::RenderWindow& window);
  void init();

  void renderMinimap(const std::vector<std::vector<ChunkStatus> >& chunkStatus) const;
  void renderTextTopLeft(const std::string& string) const;
  void renderTextTopRight(const std::string& string) const;
  void renderTextCenter(const std::string& string) const;
  void renderRectSelect() const;
  void renderLifeBars(std::set<Lion*> selection) const;

  glm::vec2 getMinimapClickCoord(float x, float y) const;
  void setRectSelect(glm::ivec4 rect);

private:
  sf::RectangleShape _rectSelect;
  sf::Font _logFont;
  sf::Text _log;
  sf::Sprite _minimapSprite;
  sf::Texture _minimapTexture;

  sf::RenderWindow& _window;
};
