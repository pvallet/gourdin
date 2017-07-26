#include "gameSandbox.h"

#include "camera.h"
#include "log.h"
#include "texturedRectangle.h"

#define LION_MIN_SPAWN_DIST 20

GameSandbox::GameSandbox(Engine& engine):
  Game::Game(engine),
  _displayLog(true),
  _huntHasStarted(false),
  _maxSimultaneousLions(5),
  _nbLions(0),
  _bestScore(0),
  _msHuntDuration(120000),
  _msCenterTextDisplayDuration(1000) {}

void GameSandbox::init() {
  Game::init();
  _interface.setTextTopCenter("Best score: 0");
}

void GameSandbox::update(int msElapsed) {
  Game::update(msElapsed);

  Log& logText = Log::getInstance();
  logText.addFPSandCamInfo(msElapsed);

  // Remove the dead elements from the selected elements
  std::vector<Lion*> toDelete;
  for (auto it = _selection.begin(); it != _selection.end(); it++) {
   if ((*it)->isDead())
     toDelete.push_back(*it);
  }
  for (size_t i = 0; i < toDelete.size(); i++) {
   _selection.erase(toDelete[i]);
  }

  // Check if the hunt has ended
  if (_huntHasStarted && _huntStart.getElapsedTime() > _msHuntDuration)
    interruptHunt();

  if (_huntHasStarted)
    _interface.setTextTopRight(getHuntText());
  else if (_displayLog)
    _interface.setTextTopRight(logText.getText());
  else
    _interface.setTextTopRight("");

  logText.clear();
}

void GameSandbox::render() const {
  Camera& cam = Camera::getInstance();

  _engine.renderToFBO();

  glViewport(0, 0, (GLint) cam.getWindowW(), (GLint) cam.getWindowH());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  _interface.renderEngine();
  _interface.renderLifeBars(_selection);
  _interface.renderRectSelect();
  _interface.renderMinimap(_engine);
  _interface.renderText();
  glViewport(0, 0, (GLint) cam.getW(), (GLint) cam.getH());
}

std::string GameSandbox::getHuntText() const {
  std::ostringstream text;

  text << "Predators: " << _nbLions << "/" << _maxSimultaneousLions << std::endl
       << "Kills: " << Lion::getNbKilled() << std::endl
       << "Time left: " << (_msHuntDuration - _huntStart.getElapsedTime()) / 1000 << std::endl;

  return text.str();
}

std::string GameSandbox::getInfoText() const {
  std::ostringstream text;

  text << "Left-Right / Q-D: " << "Rotate camera" << std::endl
       << "Up-Down    / Z-S: " << "Go forwards/backwards" << std::endl
       << "A/Return: " << "Select all lions" << std::endl
       << "B: " << "Launch benchmark" << std::endl
       << "E: " << "Change scroll speed" << std::endl;
  if (!_huntHasStarted)
    text << "H: " << "Start new hunt!" << std::endl;
  else
    text << "H: " << "Interrupt current hunt" << std::endl;
  text << "L: " << "Hide/Display log" << std::endl
       << "W: " << "Switch to wireframe display" << std::endl
       << std::endl
       << "Click on the minimap to jump there" << std::endl
       << "Right-click to make a lion appear" << std::endl
       << "Select it with the left mouse button" << std::endl
       << "Move it around with the right button" << std::endl
       << "Lshift/Rshift: " << "Make it run" << std::endl
       << "Delete: " << "Kill selected lion" << std::endl
       << std::endl
       << "Go hunt them juicy antilopes!" << std::endl;

  return Game::getInfoText() + text.str();
}

void GameSandbox::select(glm::ivec4 rect, bool add) {
  if (!add)
    _selection.clear();

  const std::set<Controllable*> controllableElements = _engine.getControllableElements();
  for (auto it = controllableElements.begin(); it != controllableElements.end(); it++) {
    Lion *lion = dynamic_cast<Lion*>(*it);
    if (lion && !lion->isDead()) {
      // controllableElements[i] is not selected yet, we can bother to calculate
      if (_selection.find(lion) == _selection.end()) {
        glm::ivec4 SpriteRect = lion->getScreenRect();

        int centerX, centerY;

        centerX = SpriteRect.x + SpriteRect.z / 2;
        centerY = SpriteRect.y + SpriteRect.w / 2;

        if (ut::contains(rect, glm::ivec2(centerX, centerY)))
          _selection.insert(lion);

        else if (   ut::contains(SpriteRect, glm::ivec2(rect.x, rect.y)) ||
                    ut::contains(SpriteRect, glm::ivec2(rect.x + rect.z, rect.y)) ||
                    ut::contains(SpriteRect, glm::ivec2(rect.x + rect.z, rect.y + rect.w)) ||
                    ut::contains(SpriteRect, glm::ivec2(rect.x, rect.y + rect.w))  ) {
          _selection.insert(lion);
        }
      }
    }
  }
}

void GameSandbox::selectAllLions() {
  _selection.clear();
  const std::set<Controllable*> controllableElements = _engine.getControllableElements();
  for (auto it = controllableElements.begin(); it != controllableElements.end(); it++) {
    Lion *lion = dynamic_cast<Lion*>(*it);
    if (lion && !lion->isDead()) {
      _selection.insert(lion);
    }
  }
}

void GameSandbox::createLion(glm::ivec2 screenTarget) {
  try {
    if (!_huntHasStarted)
      _engine.addLion(screenTarget);

    else {
      if (_nbLions >= _maxSimultaneousLions)
        throw std::runtime_error("Maximum number of predators reached");

      _engine.addLion(screenTarget, LION_MIN_SPAWN_DIST);
      _nbLions++;
    }
  } catch (const std::runtime_error& e) {
    displayError(e.what());
  }
}

void GameSandbox::moveSelection(glm::ivec2 screenTarget) {
  glm::vec2 target = _engine.get2DCoord(screenTarget);

  for(auto it = _selection.begin(); it != _selection.end(); ++it) {
    Controllable* ctrl = dynamic_cast<Controllable*>(*it);
    if (ctrl) {
      ctrl->setTarget(target);
    }
  }
}

void GameSandbox::goBackToSelection() {
  if (!_selection.empty()) {
    glm::vec2 barycenter;
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
  for (auto it = _selection.begin(); it != _selection.end(); ++it) {
    (*it)->beginRunning();
  }
}

void GameSandbox::switchLionsRun() {
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
  if (!_selection.empty()) {
    (*_selection.begin())->die();
    _nbLions--;
  }
}

void GameSandbox::clearLog() const {
  Log& logText = Log::getInstance();
  logText.clear();
}

void GameSandbox::benchmark() {
  Camera& cam = Camera::getInstance();

  cam.setPointedPos(glm::vec2(CHUNK_SIZE / 2, CHUNK_SIZE / 2));
  cam.setValues(5000, 225.f, INIT_PHI);

  Clock frameClock(INDEPENDENT);

  for (size_t i = 0; i < 100; i++) {
    cam.setPointedPos(glm::vec2(CHUNK_SIZE / 2 + i / 100.f * CHUNK_SIZE * (NB_CHUNKS-1),
                                CHUNK_SIZE / 2 + i / 100.f * CHUNK_SIZE * (NB_CHUNKS-1)));
    update(0);
    render();
  }

  int msTotalElapsed = frameClock.getElapsedTime();

  SDL_Log("Rendered 100 frames in %d milliseconds.",  msTotalElapsed);
  SDL_Log("Average FPS: %f", 1.f / msTotalElapsed * 100 * 1000);

  _engine.resetCamera();
}

void GameSandbox::interruptHunt() {
  if (_huntHasStarted) {
    _huntHasStarted = false;
    if (Lion::getNbKilled() > _bestScore)
      _bestScore = Lion::getNbKilled();

    Log& logText = Log::getInstance();
    logText.clear();

    std::ostringstream bestScoreText;
    bestScoreText << "Best score: " << _bestScore;
    _interface.setTextTopCenter(bestScoreText.str());

    _interface.setTextTopRight("");
    _interface.setTextTopLeft(getInfoText());

    std::ostringstream scoreText;
    scoreText << "Kills: " << Lion::getNbKilled();
    _interface.setTextCenter(scoreText.str(), _msCenterTextDisplayDuration);
  }
}

void GameSandbox::startNewHunt() {
  if (!_huntHasStarted) {
    std::vector<igMovingElement*> toDelete;

    const std::set<Controllable*>& lions = _engine.getControllableElements();
    for (auto it = lions.begin(); it != lions.end(); it++) {
      if (dynamic_cast<Lion*>(*it))
        toDelete.push_back(*it);
    }

    const std::set<Controllable*>& deadLions = _engine.getDeadControllableElements();
    for (auto it = deadLions.begin(); it != deadLions.end(); it++) {
      if (dynamic_cast<Lion*>(*it))
        toDelete.push_back(*it);
    }

    _engine.deleteElements(toDelete);
    _selection.clear();
    _nbLions = 0;
    Lion::resetNbKilled();
    _huntHasStarted = true;
    _huntStart.restart();

    _interface.setTextTopLeft(getInfoText());
    _interface.setTextCenter("Hunt Starts!", _msCenterTextDisplayDuration);
  }
}
