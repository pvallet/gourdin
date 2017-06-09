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
  _engine.update(elapsed);
}

void GameSandbox::render() const {
  _engine.render();

#ifndef CORE_PROFILE
  _window.pushGLStates();

  _engine.renderLifeBars(_window);
  _interface.renderRectSelect();
  _interface.renderMinimap(_engine);

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
       << "M: " << "Switch to Sandbox mode" << std::endl
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
