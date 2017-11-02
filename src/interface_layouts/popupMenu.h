#pragma once

#include <glm/glm.hpp>

#include <string>

#include "coloredRectangles.h"
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

    glm::ivec2 getTextSize();

  private:
    Text _text;
    TCallbackFunc _callback;
    bool _highlighted;
  };

public:
  PopupMenu (Game* game);

  void setTitle(const std::string& title);
  void addEntry(const std::string& name, TCallbackFunc callback);

  void bindShadersAndDraw() const;

  void create(glm::ivec2 clickPos);
  void updateHighlight(const glm::ivec2& mousePos);
  void triggerAndDestroy(const glm::ivec2& clickPos);

private:
  glm::ivec2 getMenuSize();

  Game* _game;

  bool _isDisplaying;

  ColoredRectangles _frame;
  ColoredRectangles _background;

  std::vector<MenuEntry> _menuEntries;
};
