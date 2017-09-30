#pragma once

#include <glm/glm.hpp>

#include <string>

class Game;

class PopupMenu {
public:
  PopupMenu (Game* game) {}

  void setTitle(const std::string& title);
  void addEntry(const std::string& name, void (Game::*callback) (glm::ivec2));

  void create(glm::ivec2 clickPos);
  void updateHighlight(glm::ivec2 mousePos);
  void triggerAndDestroy(glm::ivec2 clickPos);

private:
};
