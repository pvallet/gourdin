#pragma once

#include <SDL2pp/SDL2pp.hh>

#include <set>

#include "chronometer.h"
#include "engine.h"
#include "opengl.h"
#include "text.h"
#include "texturedRectangle.h"

class Interface {
public:
  Interface() {}
  void init();

  void renderEngine() const;
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

  inline void setEngineTexture(GLuint texID) {_texRectEngine.reset(new TexturedRectangle(texID, -1, -1, 2, 2));}
  void setRectSelect(glm::ivec4 rect);

private:
  bool _androidBuild;

  // sf::RectangleShape _rectSelect;
  Text _textTopLeft;
  Text _textTopRight;
  Text _textTopCenter;
  Text _textCenter;
  Text _textBottomCenter;
  // sf::Sprite _minimapSprite;
  // sf::Texture _minimapTexture;

  Chronometer _textCenterChrono;
  Chronometer _textBottomCenterChrono;

  std::unique_ptr<const TexturedRectangle> _texRectEngine;
};
