#include "popupMenu.h"

#include "camera.h"
#include "game.h"
#include "interfaceParameters.h"

#define SPACE_BETWEEN_ENTRIES 3

glm::ivec2 PopupMenu::MenuEntry::_clickPos;

PopupMenu::MenuEntry::MenuEntry(std::string name, TCallbackFunc callback):
  _callback(callback),
  _highlight(InterfaceParameters::getInstance().colorBackground()),
  _highlighted(false) {

  const InterfaceParameters& iParams = InterfaceParameters::getInstance();
  _text.setText(name, iParams.sizeTextSmall());
}

void PopupMenu::MenuEntry::bindShaderAndDraw() const {
  if (_highlighted)
    _highlight.bindShaderAndDraw();
  _text.bindShaderAndDraw();
}

void PopupMenu::MenuEntry::triggerAndDestroy(Game* game) {
  (game->*_callback)(_clickPos);
}

void PopupMenu::MenuEntry::setAllocatedRect(const glm::ivec4& allocatedRect) {
  _allocatedRect = allocatedRect;
  _highlight.setRectangles(allocatedRect);
  _text.setPosition(glm::ivec2(allocatedRect.x, allocatedRect.y));
}

PopupMenu::PopupMenu(Game* game):
  _game(game),
  _highlightedIndex(0),
  _isDisplaying(false),
  _frame(InterfaceParameters::getInstance().colorFrame(), false),
  _background(InterfaceParameters::getInstance().colorBackground(), true) {}

void PopupMenu::setTitle(const std::string& title) {

}

void PopupMenu::addEntry(const std::string& name, TCallbackFunc callback) {
  _menuEntries.push_back(PopupMenu::MenuEntry(name, callback));
}

void PopupMenu::bindShadersAndDraw() const {
  if (_isDisplaying) {
    _background.bindShaderAndDraw();

    for (size_t i = 0; i < _menuEntries.size(); i++) {
      _menuEntries[i].bindShaderAndDraw();
    }

    _frame.bindShaderAndDraw();
  }
}

glm::ivec2 PopupMenu::getMenuSize() {
  glm::ivec2 totalSize;

  for (size_t i = 0; i < _menuEntries.size(); i++) {
    glm::ivec2 currentSize = _menuEntries[i].getTextSize();

    totalSize.x = std::max(totalSize.x, currentSize.x);
    totalSize.y += currentSize.y;
  }

  totalSize.y += _menuEntries.size() * SPACE_BETWEEN_ENTRIES;

  return totalSize;
}

void PopupMenu::create(const glm::ivec2& clickPos) {
  if (!_menuEntries.empty()) {
    MenuEntry::_clickPos = clickPos;

    glm::ivec4 menuRectangle(clickPos - _menuEntries.front().getTextSize() / 2, getMenuSize());

    _frame.setRectangles(menuRectangle);
    _background.setRectangles(menuRectangle);

    glm::ivec4 currentAllocatedRect = menuRectangle;
    currentAllocatedRect.w /= _menuEntries.size();

    for (size_t i = 0; i < _menuEntries.size(); i++) {
      _menuEntries[i].setAllocatedRect(currentAllocatedRect);
      currentAllocatedRect.y += currentAllocatedRect.w;
    }

    _isDisplaying = true;

    updateHighlight(clickPos);
  }
}

void PopupMenu::updateHighlight(const glm::ivec2& mousePos) {
  if (!_menuEntries.empty()) {
    if (mousePos.y < _menuEntries.front().getAllocatedRect().y) {
      highlightIndex(0);
      return;
    }
    else if (mousePos.y > _menuEntries.back().getAllocatedRect().y) {
      highlightIndex(_menuEntries.size() - 1);
      return;
    }

    for (size_t i = 0; i < _menuEntries.size(); i++) {
      glm::ivec4 allocatedRect = _menuEntries[i].getAllocatedRect();
      if (allocatedRect.y + allocatedRect.w > mousePos.y) {
        highlightIndex(i);
        return;
      }
    }
  }
}

void PopupMenu::highlightIndex(size_t index) {
  _highlightedIndex = index;

  for (size_t i = 0; i < _menuEntries.size(); i++) {
    if (i == index)
      _menuEntries[i].setHighlighted(true);
    else
      _menuEntries[i].setHighlighted(false);
  }
}

void PopupMenu::triggerAndDestroy(const glm::ivec2& clickPos) {
  updateHighlight(clickPos);
  _menuEntries[_highlightedIndex].triggerAndDestroy(_game);
  _isDisplaying = false;
}
