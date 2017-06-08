#include "gameSandbox.h"

#include "camera.h"

GameSandbox::GameSandbox (Engine& engine, Interface& interface):
  _engine(engine),
  _interface(interface) {}

void GameSandbox::moveSelection(sf::Vector2i screenPos) {
  if (_engine.getSelection().empty())
    _engine.addLion(screenPos);
  else
    _engine.moveSelection(screenPos);
}

void GameSandbox::goBackToSelection() {
  std::set<Controllable*> sel = _engine.getSelection();

  if (!sel.empty()) {
    sf::Vector2f barycenter;
    float nbSelected = 0;

    for (auto it = sel.begin(); it != sel.end(); ++it) {
      Lion* lion = dynamic_cast<Lion*>(*it);
      if (lion) {
        barycenter += lion->getPos();
        nbSelected++;
      }
    }
    Camera& cam = Camera::getInstance();
    cam.setPointedPos(barycenter / nbSelected);
  }
}

void GameSandbox::makeLionsRun() {
  std::set<Controllable*> sel = _engine.getSelection();

  bool makeThemAllRun = false;
  bool generalStrategyChosen = false;
  for (auto it = sel.begin(); it != sel.end(); ++it) {
    Lion* lion = dynamic_cast<Lion*>(*it);
    if (lion) {
      if (!generalStrategyChosen) {
        generalStrategyChosen = true;
        makeThemAllRun = !lion->isRunning();
      }
      if (generalStrategyChosen) {
        if (makeThemAllRun)
          lion->beginRunning();
        else
          lion->beginWalking();
      }
    }
  }
}

void GameSandbox::killLion() {
  std::set<Controllable*> sel = _engine.getSelection();

  for (auto it = sel.begin(); it != sel.end(); it++) {
    (*it)->die();
  }
}
