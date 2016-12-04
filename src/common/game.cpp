#include "game.h"

#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"

#include <ctime>

#define CHUNK_BEGIN_X 14
#define CHUNK_BEGIN_Y 16

Game::Game() :
  _terrainShader("src/shaders/heightmap.vert", "src/shaders/heightmap.frag"),
  _igEShader ("src/shaders/igElement.vert", "src/shaders/igElement.frag"),
  _terrainTransitionShader("src/shaders/terrainTexTransitions.vert", "src/shaders/terrainTexTransitions.frag"),
  _contentGenerator(_map),
  _ocean(0.5) {}

void Game::init() {
  srand(time(NULL));

  _terrainShader.load();
  _igEShader.load();
  _terrainTransitionShader.load();

  _terrainTexManager.loadFolder(NB_BIOMES, "res/terrain/");
  _map.load("res/map/");
  _map.feedGeometryData(_terrainGeometry);
  _terrainGeometry.computeNormals();

  _ocean.setTexIndex(_terrainTexManager.getTexID(OCEAN));

  std::vector<ChunkStatus> initializer(NB_CHUNKS, NOT_GENERATED);
  _chunkStatus.resize(NB_CHUNKS, initializer);

  for (size_t i = 0; i < NB_CHUNKS; i++) {
    std::vector<std::unique_ptr<Chunk> > initializer2(NB_CHUNKS);
    _terrain.push_back(std::move(initializer2));
  }

  _contentGenerator.init();
  _contentGenerator.saveToImage("contents");

  Camera& cam = Camera::getInstance();
  cam.setPointedPos(sf::Vector2f(CHUNK_BEGIN_X * CHUNK_SIZE + CHUNK_SIZE / 2,
                                 CHUNK_BEGIN_Y * CHUNK_SIZE + CHUNK_SIZE / 2));

  appendNewElements(_contentGenerator.genHerd(
                    sf::Vector2f(CHUNK_BEGIN_X * CHUNK_SIZE + CHUNK_SIZE / 2,
                                 CHUNK_BEGIN_Y * CHUNK_SIZE + CHUNK_SIZE / 2), 20));
}

void Game::generateChunk(size_t x, size_t y) {
  Chunk* newChunk = new Chunk(x, y, _terrainTexManager, _map);
  newChunk->generate();
  _terrain[x][y] = std::unique_ptr<Chunk>(newChunk);
  _chunkStatus[x][y] = EDGE;
  appendNewElements(_contentGenerator.genForestsInChunk(x, y));
}

sf::Vector2i Game::neighbour(size_t x, size_t y, size_t index) const {
  assert(index < 4 && "Error in Game::neighbour: Index out of bounds");
  switch(index) {
    case 0:
      return sf::Vector2i(x-1,y);
      break;
    case 1:
      return sf::Vector2i(x+1,y);
      break;
    case 2:
      return sf::Vector2i(x,y-1);
      break;
    case 3:
      return sf::Vector2i(x,y+1);
      break;
  }
}

void Game::generateNeighbourChunks(size_t x, size_t y) {
  if (_chunkStatus[x][y] == EDGE) {
    sf::Vector2i tmp;
    for (size_t i = 0; i < 4; i++) {
      tmp = neighbour(x,y,i);

      // We check if the neighbour chunk is within the map space.
      // If not, we should generate oceans TODO
      if (tmp.x >= 0 && tmp.x < _chunkStatus.size() && tmp.y >= 0 && tmp.y < _chunkStatus.size()) {
        if (_chunkStatus[tmp.x][tmp.y] == NOT_GENERATED) {
          // If the chunk to be generated is not handled by the map, we don't do anything
          if (tmp.x < 0 || tmp.x >= NB_CHUNKS || tmp.y < 0 || tmp.y >= NB_CHUNKS);


          else if (_map.isOcean(tmp.x, tmp.y))
            _chunkStatus[tmp.x][tmp.y] = NOT_GENERATED;

          else
            generateChunk(tmp.x,tmp.y);
        }
      }
    }
  }

  _chunkStatus[x][y] = NOT_VISIBLE;
}

void Game::appendNewElements(std::vector<igElement*> elems) {
  for (size_t i = 0; i < elems.size(); i++)
    _igElements.push_back(std::unique_ptr<igElement>(elems[i]));

  if (!elems.empty() && dynamic_cast<igMovingElement*>(elems.front())) {
    for (size_t i = 0; i < elems.size(); i++) {
      _igMovingElements.insert(dynamic_cast<igMovingElement*>(elems[i]));
    }
  }
}

void Game::updateMovingElementsStates() {
  // Remove the dead elements from the selection and the interacting elements
  std::vector<igMovingElement*> toDelete;
  for (auto it = _selectedElmts.begin(); it != _selectedElmts.end(); it++) {
    if ((*it)->isDead())
      toDelete.push_back(*it);
  }
  for (size_t i = 0; i < toDelete.size(); i++) {
    _selectedElmts.erase((Controllable*) toDelete[i]);
  }
  toDelete.clear();

  for (auto it = _igMovingElements.begin(); it != _igMovingElements.end(); it++) {
    if ((*it)->isDead())
      toDelete.push_back(*it);
  }
  for (size_t i = 0; i < toDelete.size(); i++) {
    _igMovingElements.erase(toDelete[i]);
  }

  // Compute moving elements interactions
  for (auto it = _igMovingElements.begin(); it != _igMovingElements.end(); it++) {
    Antilope* atlp;
    Lion* lion;

    if (atlp = dynamic_cast<Antilope*>(*it))
      atlp->updateState(_igMovingElements);

    else if (lion = dynamic_cast<Lion*>(*it))
      lion->kill(_igMovingElements);
  }
}

void Game::update(sf::Time elapsed) {
  Camera& cam = Camera::getInstance();
  int camPosX = cam.getPointedPos().x < 0 ? cam.getPointedPos().x / CHUNK_SIZE - 1 : cam.getPointedPos().x / CHUNK_SIZE;
  int camPosY = cam.getPointedPos().y < 0 ? cam.getPointedPos().y / CHUNK_SIZE - 1 : cam.getPointedPos().y / CHUNK_SIZE;

  // Update camera
  if (_chunkStatus[camPosX][camPosY] == NOT_GENERATED) {
    generateChunk(camPosX, camPosY);
  }

  cam.setHeight( _terrain[camPosX][camPosY]
    ->getHeight(cam.getPointedPos().x - CHUNK_SIZE * camPosX,
                cam.getPointedPos().y - CHUNK_SIZE * camPosY));

  // Update terrains
  for (size_t i = 0; i < NB_CHUNKS; i++) {
    for (size_t j = 0; j < NB_CHUNKS; j++) {
      if (_chunkStatus[i][j] != NOT_GENERATED) {
        _terrain[i][j]->computeCulling();

        if (_chunkStatus[i][j] == EDGE) {
          if (_terrain[i][j]->isVisible())
            generateNeighbourChunks(i,j);
        }

        else {
          if (_terrain[i][j]->isVisible())
            _chunkStatus[i][j] = VISIBLE;
          else
            _chunkStatus[i][j] = NOT_VISIBLE;
        }
      }
    }
  }

  updateMovingElementsStates();

  _visibleElmts.clear();

  // Update in game elements characteristics
  for (unsigned int i = 0 ; i < _igElements.size() ; i++) {
    _igElements[i]->update(elapsed, cam.getTheta()); // Choose the right sprite and update pos

    int chunkPosX = _igElements[i]->getPos().x / CHUNK_SIZE;
    int chunkPosY = _igElements[i]->getPos().y / CHUNK_SIZE;

    if (chunkPosX == NB_CHUNKS)
      chunkPosX--;
    if (chunkPosY == NB_CHUNKS)
      chunkPosY--;

    if (_chunkStatus[chunkPosX][chunkPosY] == VISIBLE) {

      // No test yet to see if the element can move to its new pos (no collision)
      float newHeight =   _terrain[chunkPosX][chunkPosY]
                           ->getHeight(_igElements[i]->getPos().x - (int) CHUNK_SIZE * chunkPosX,
                                       _igElements[i]->getPos().y - (int) CHUNK_SIZE * chunkPosY);

      // Calculate new corners
      glm::vec3 corners3[4];
      float width = _igElements[i]->getSize().x;

      corners3[0] = glm::vec3(  _igElements[i]->getPos().x - sin(cam.getTheta()*M_PI/180.f)*width/2,
                                _igElements[i]->getPos().y + cos(cam.getTheta()*M_PI/180.f)*width/2,
                                newHeight + _igElements[i]->getSize().y);

      corners3[1] = glm::vec3(  _igElements[i]->getPos().x + sin(cam.getTheta()*M_PI/180.f)*width/2,
                                _igElements[i]->getPos().y - cos(cam.getTheta()*M_PI/180.f)*width/2,
                                newHeight + _igElements[i]->getSize().y);

      corners3[2] = glm::vec3(  _igElements[i]->getPos().x + sin(cam.getTheta()*M_PI/180.f)*width/2,
                                _igElements[i]->getPos().y - cos(cam.getTheta()*M_PI/180.f)*width/2,
                                newHeight);

      corners3[3] = glm::vec3(  _igElements[i]->getPos().x - sin(cam.getTheta()*M_PI/180.f)*width/2,
                                _igElements[i]->getPos().y + cos(cam.getTheta()*M_PI/180.f)*width/2,
                                newHeight);

      _igElements[i]->set3DCorners(corners3);

      // Calculate their projections
      glm::mat4 viewProj = cam.getViewProjectionMatrix();

      glm::vec3 cornersProjNorm[4];
      for (size_t i = 0; i < 4; i++) {
        cornersProjNorm[i] = glm::project(corners3[i], glm::mat4(1.f), viewProj,
          glm::vec4(0,0,cam.getW(),cam.getH()));
      }

      float left  = cornersProjNorm[0].x;
      float top   = cornersProjNorm[0].y;
      float right = cornersProjNorm[1].x;
      float bot   = cornersProjNorm[3].y;
      float depth = cornersProjNorm[3].z;

      top = cam.getH()-top;
      bot = cam.getH()-bot;

      sf::IntRect cornersRect((int) left, (int) top, (int) right-left, (int) bot-top);

      _igElements[i]->set2DCorners(cornersRect);
      _igElements[i]->setDepth(depth);

      _visibleElmts.insert(_igElements[i].get());
    }
  }
}

void Game::render() const {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glDisable(GL_DEPTH_TEST);
  glUseProgram(_terrainShader.getProgramID());
  _terrainShader.sendModelMatrix(glm::mat4(1.f));
  _ocean.draw();
  glEnable(GL_DEPTH_TEST);

  // Chunk transitions

  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glUseProgram(_terrainTransitionShader.getProgramID());

  for (size_t i = 0; i < NB_CHUNKS; i++) {
    for (size_t j = 0; j < NB_CHUNKS; j++) {
      if (_chunkStatus[i][j] == VISIBLE) {
        glm::mat4 modelview = glm::translate(glm::mat4(1.f),
          glm::vec3(CHUNK_SIZE * i, CHUNK_SIZE * j, 0.f)
        );

        _terrainTransitionShader.sendModelMatrix(modelview);
        _terrain[i][j]->drawTransitions();
      }
    }
  }

  glDisable(GL_BLEND);

  // Chunk

  glUseProgram(_terrainShader.getProgramID());

  for (size_t i = 0; i < NB_CHUNKS; i++) {
    for (size_t j = 0; j < NB_CHUNKS; j++) {
      if (_chunkStatus[i][j] == VISIBLE) {

        glm::mat4 modelview = glm::translate(glm::mat4(1.f),
          glm::vec3(CHUNK_SIZE * i, CHUNK_SIZE * j, 0.f)
        );

        _terrainShader.sendModelMatrix(modelview);

        _terrain[i][j]->draw();
      }
    }
  }

  // igElements

  glUseProgram(_igEShader.getProgramID());
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  for (auto it = _visibleElmts.begin() ; it != _visibleElmts.end() ; ++it) {
    _terrainShader.sendModelMatrix(glm::mat4(1.f));
    (*it)->draw();
  }

  glDisable(GL_BLEND);

  glUseProgram(0);

  GL_CHECK_ERROR();
}

void Game::select(sf::IntRect rect, bool add) {
  if (!add)
    _selectedElmts.clear();

  for (unsigned int i = 0 ; i < _igElements.size() ; i++) {
    Controllable *ctrl = dynamic_cast<Controllable*>(_igElements[i].get());
    if (ctrl) {
      if (_selectedElmts.find(ctrl) == _selectedElmts.end()) { // _igElements[i] is not selected yet, we can bother to calculate
        sf::IntRect c = ctrl->get2DCorners();

        int centerX, centerY;

        centerX = c.left + c.width / 2;
        centerY = c.top + c.height / 2;

        if (rect.contains(centerX, centerY)) {
          if (dynamic_cast<Lion*>(ctrl))
            _selectedElmts.insert(ctrl);
        }

        else if (   c.contains(rect.left, rect.top) ||
                    c.contains(rect.left + rect.width, rect.top) ||
                    c.contains(rect.left + rect.width, rect.top + rect.height) ||
                    c.contains(rect.left, rect.top + rect.height)  ) {
          if (dynamic_cast<Lion*>(ctrl))
            _selectedElmts.insert(ctrl);
        }
      }
    }
  }
}

void Game::moveSelection(sf::Vector2i screenTarget) {
  sf::Vector2f target = get2DCoord(screenTarget);

  for(auto it = _selectedElmts.begin(); it != _selectedElmts.end(); ++it) {
    Controllable* ctrl;
    if (ctrl = dynamic_cast<Controllable*>(*it)) {
      ctrl->setTarget(target);
    }
  }
}

void Game::moveCamera(sf::Vector2f newAimedPos) {
  generateChunk(newAimedPos.x / CHUNK_SIZE, newAimedPos.y / CHUNK_SIZE);
  Camera& cam = Camera::getInstance();
  cam.setPointedPos(newAimedPos);
}

void Game::addLion(sf::Vector2i screenTarget) {
  appendNewElements(_contentGenerator.genLion(get2DCoord(screenTarget)));
}

sf::Vector2f Game::get2DCoord(sf::Vector2i screenTarget) const {
  Camera& cam = Camera::getInstance();
  screenTarget.y = cam.getH() - screenTarget.y; // Inverted coordinates

  GLfloat d;
  glReadPixels(screenTarget.x, screenTarget.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &d);

  glm::vec3 modelCoord = glm::unProject(glm::vec3(screenTarget.x, screenTarget.y,d),
    glm::mat4(1.f), cam.getViewProjectionMatrix(),
    glm::vec4(0,0, cam.getW(), cam.getH()));

  return sf::Vector2f( modelCoord.x, modelCoord.y);
}
