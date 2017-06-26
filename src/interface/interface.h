#pragma once

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include "opengl.h"

#include <set>

#include "chronometer.h"
#include "engine.h"

class Interface {
public:
  Interface(sf::RenderWindow& window);
  void init();

  void renderMinimap(const std::vector<std::vector<ChunkStatus> >& chunkStatus) const;
  void renderText() const;
  void renderRectSelect() const;
  void renderLifeBars(std::set<Lion*> selection) const;

  glm::vec2 getMinimapClickCoord(float x, float y) const;
  void setTextTopLeft(const std::string& string);
  void setTextTopRight(const std::string& string);
  void setTextTopCenter(const std::string& string);
  void setTextCenter(const std::string& string, int msDuration = -1);
  void setTextBottomCenter(const std::string& string, int msDuration = -1);
  void clearText();

  void setRectSelect(glm::ivec4 rect);

private:
  sf::RectangleShape _rectSelect;
  sf::Font _textFont;
  sf::Text _textTopLeft;
  sf::Text _textTopRight;
  sf::Text _textTopCenter;
  sf::Text _textCenter;
  sf::Text _textBottomCenter;
  sf::Sprite _minimapSprite;
  sf::Texture _minimapTexture;

  Chronometer _textCenterChrono;
  Chronometer _textBottomCenterChrono;

  sf::RenderWindow& _window;
};
