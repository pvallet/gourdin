#pragma once

#include <glm/glm.hpp>

#include <string>

#include "coloredRectangles.h"
#include "interfaceParameters.h"
#include "text.h"

class Game;

typedef void (Game::*TCallbackFunc)(glm::ivec2);

class PopupMenu {
  class MenuEntry {
  public:
    MenuEntry(std::string name, void (Game::*callback) (glm::ivec2));

    void bindShaderAndDraw() const;
    void triggerAndDestroy(Game* game);
    inline void setHighlighted(bool highlighted) {_highlighted = highlighted;}
    void setAllocatedRect(const glm::ivec4& allocatedRect);

    inline glm::ivec2 getTextSize() const {return _text.getSize();}
    inline glm::ivec4 getAllocatedRect() const {return _allocatedRect;}

    static glm::ivec2 _clickPos;

  private:
    Text _text;
    TCallbackFunc _callback;
    ColoredRectangles _highlight;

    bool _highlighted;

    glm::ivec4 _allocatedRect;
  };

public:
  PopupMenu (Game* game);

  void addEntry(const std::string& name, TCallbackFunc callback);

  void bindShadersAndDraw() const;

  void create(const glm::ivec2& clickPos);
  void updateHighlight(const glm::ivec2& mousePos);
  void triggerAndDestroy(const glm::ivec2& clickPos);

private:
  void highlightIndex(int index);
  glm::ivec2 getMenuSize();

  Game* _game;

  int _highlightedIndex;
  bool _isDisplaying;

  ColoredRectangles _frame;
  ColoredRectangles _background;

  std::vector<MenuEntry> _menuEntries;
};
