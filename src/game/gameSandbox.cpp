#include "gameSandbox.h"

#include "camera.h"

GameSandbox::GameSandbox (sf::RenderWindow& window, Engine& engine, Interface& interface):
  _displayLog(true),
  _window(window),
  _engine(engine),
  _interface(interface) {}

void GameSandbox::update(sf::Time elapsed) {
  LogText& logText = LogText::getInstance();
  logText.addFPSandCamInfo(elapsed);

  // Remove the dead elements from the selected elements
  std::vector<Lion*> toDelete;
  for (auto it = _selection.begin(); it != _selection.end(); it++) {
   if ((*it)->isDead())
     toDelete.push_back(*it);
  }
  for (size_t i = 0; i < toDelete.size(); i++) {
   _selection.erase(toDelete[i]);
  }

  _engine.update(elapsed);
}

void GameSandbox::render() const {
  _engine.render();

#ifndef CORE_PROFILE
  _window.pushGLStates();

  _interface.renderLifeBars(_selection);
  _interface.renderRectSelect();
  _interface.renderMinimap(_engine.getChunkStatus());

  _interface.renderTextTopLeft(getInfoText());

  if (_displayLog) {
    LogText& logText = LogText::getInstance();
    _interface.renderTextTopRight(logText.getText());
    logText.clear();
  }

  _window.popGLStates();
#endif

  _window.display();
}

std::string GameSandbox::getInfoText() const {
  std::ostringstream text;

  text << "Esc: " << "Quit engine" << std::endl
       << "M: " << "Switch to Game mode" << std::endl
       << "Left-Right: " << "Rotate camera" << std::endl
       << "Up-Down:    " << "Go forwards/backwards" << std::endl
       << "B: " << "Launch benchmark" << std::endl
       << "L: " << "Hide/Display log" << std::endl;
  if (_scrollSpeedSlow)
    text << "S: " << "Set scroll speed to 'fast'" << std::endl;
  else
    text << "S: " << "Set scroll speed to 'slow'" << std::endl;
  text << "W: " << "Switch to wireframe display" << std::endl
      << std::endl
      << "Click on the minimap to jump there" << std::endl
      << "Right-click to make a lion appear" << std::endl
      << "Select it with the left mouse button" << std::endl
      << "Move it around with the right button" << std::endl
      << "Lshift: " << "Make it run" << std::endl
      << std::endl
      << "Go hunt them juicy antilopes!" << std::endl;

  return text.str();
}

void GameSandbox::select(sf::IntRect rect, bool add) {
  if (!add)
    _selection.clear();

  const std::set<Controllable*> controllableElements = _engine.getControllableElements();
  for (auto it = controllableElements.begin(); it != controllableElements.end(); it++) {
    Lion *lion = dynamic_cast<Lion*>(*it);
    if (lion && !lion->isDead()) {
      // controllableElements[i] is not selected yet, we can bother to calculate
      if (_selection.find(lion) == _selection.end()) {
        sf::IntRect SpriteRect = lion->getScreenCoord();

        int centerX, centerY;

        centerX = SpriteRect.left + SpriteRect.width / 2;
        centerY = SpriteRect.top + SpriteRect.height / 2;

        if (rect.contains(centerX, centerY))
          _selection.insert(lion);

        else if (   SpriteRect.contains(rect.left, rect.top) ||
                    SpriteRect.contains(rect.left + rect.width, rect.top) ||
                    SpriteRect.contains(rect.left + rect.width, rect.top + rect.height) ||
                    SpriteRect.contains(rect.left, rect.top + rect.height)  ) {
          _selection.insert(lion);
        }
      }
    }
  }
}

void GameSandbox::moveSelection(sf::Vector2i screenTarget) {
  if (_selection.empty())
    _engine.addLion(screenTarget);
  else {
    sf::Vector2f target = Engine::get2DCoord(screenTarget);

    for(auto it = _selection.begin(); it != _selection.end(); ++it) {
      Controllable* ctrl = dynamic_cast<Controllable*>(*it);
      if (ctrl) {
        ctrl->setTarget(target);
      }
    }
  }
}

void GameSandbox::goBackToSelection() {
  if (!_selection.empty()) {
    sf::Vector2f barycenter;
    float nbSelected = 0;

    for (auto it = _selection.begin(); it != _selection.end(); ++it) {
      barycenter += (*it)->getPos();
      nbSelected++;
    }
    Camera& cam = Camera::getInstance();
    cam.setPointedPos(barycenter / nbSelected);
  }
}

void GameSandbox::makeLionsRun() {
  bool makeThemAllRun = false;
  bool generalStrategyChosen = false;
  for (auto it = _selection.begin(); it != _selection.end(); ++it) {
    if (!generalStrategyChosen) {
      generalStrategyChosen = true;
      makeThemAllRun = !(*it)->isRunning();
    }
    if (generalStrategyChosen) {
      if (makeThemAllRun)
        (*it)->beginRunning();
      else
        (*it)->beginWalking();
    }
  }
}

void GameSandbox::killLion() {
  if (!_selection.empty())
    (*_selection.begin())->die();
}

void GameSandbox::clearLog() const {
  LogText& logText = LogText::getInstance();
  logText.clear();
}

void GameSandbox::benchmark() {
  Camera& cam = Camera::getInstance();

  cam.setPointedPos(sf::Vector2f(CHUNK_SIZE / 2, CHUNK_SIZE / 2));
  cam.setValues(5000, 225.f, INIT_PHI);

  sf::Time totalElapsed, elapsed;
  sf::Clock frameClock;

  for (size_t i = 0; i < 100; i++) {
    elapsed = frameClock.restart();
    totalElapsed += elapsed;
    cam.setPointedPos(sf::Vector2f(CHUNK_SIZE / 2 + i / 100.f * CHUNK_SIZE * (NB_CHUNKS-1),
                                   CHUNK_SIZE / 2 + i / 100.f * CHUNK_SIZE * (NB_CHUNKS-1)));
    update(elapsed);
    render();
  }

  std::cout << "Rendered " << 100 << " frames in " << totalElapsed.asMilliseconds() << " milliseconds." << std::endl;
  std::cout << "Average FPS: " << 1.f / totalElapsed.asSeconds() * 100 << std::endl;

  _engine.resetCamera();
}
