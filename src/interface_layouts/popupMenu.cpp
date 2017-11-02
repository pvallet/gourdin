#include "popupMenu.h"

#include "camera.h"
#include "interfaceParameters.h"

PopupMenu::MenuEntry::MenuEntry(std::string name, TCallbackFunc callback):
  _callback(callback),
  _highlighted(false) {

  const InterfaceParameters& iParams = InterfaceParameters::getInstance();
  _text.setText(name, iParams.sizeTextSmall());
}

void PopupMenu::MenuEntry::bindShaderAndDraw() const {
  _text.bindShaderAndDraw();
}

void PopupMenu::MenuEntry::triggerAndDestroy(Game* game) {

}

void PopupMenu::MenuEntry::setAllocatedRect(const glm::ivec4& allocatedRect) {
  _text.setPosition(glm::ivec2(allocatedRect.x, allocatedRect.y));
}

glm::ivec2 PopupMenu::MenuEntry::getTextSize() {
  return _text.getSize();
}

PopupMenu::PopupMenu(Game* game):
  _game(game),
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

  return totalSize;
}

void PopupMenu::create(glm::ivec2 clickPos) {
  if (!_menuEntries.empty()) {
    clickPos -= _menuEntries.front().getTextSize() / 2;
    glm::ivec4 menuRectangle(clickPos, getMenuSize());

    _frame.setRectangles(menuRectangle);
    _background.setRectangles(menuRectangle);

    glm::ivec4 currentAllocatedRect = menuRectangle;
    currentAllocatedRect.w /= _menuEntries.size();

    for (size_t i = 0; i < _menuEntries.size(); i++) {
      _menuEntries[i].setAllocatedRect(currentAllocatedRect);
      currentAllocatedRect.y += currentAllocatedRect.w;
    }

    _isDisplaying = true;
  }
}

void PopupMenu::updateHighlight(const glm::ivec2& mousePos) {

}

void PopupMenu::triggerAndDestroy(const glm::ivec2& clickPos) {
  _isDisplaying = false;
}
