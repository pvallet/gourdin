#include "game.h"

#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"

#include <ctime>

#define CHUNK_BEGIN_X 14
#define CHUNK_BEGIN_Y 16

struct compDepth {
  bool operator()(const igElement* a, const igElement* b) const {
    return a->getDepth() > b->getDepth();
  }
} compDepthObj;

Game::Game() :
  _wireframe(false),
  _terrainShader("src/shaders/heightmap.vert", "src/shaders/heightmap.frag"),
  _igEShader ("src/shaders/igElement.vert", "src/shaders/igElement.frag"),
  _contentGenerator(_terrainGeometry),
  _ocean(2) {}

void Game::init() {
  srand(time(NULL));

  _terrainShader.load();
  _igEShader.load();

  _terrainTexManager.loadFolder(NB_BIOMES, "res/terrain/");
  _map.load("res/map/");
  _map.feedGeometryData(_terrainGeometry);

  _ocean.setTexIndex(_terrainTexManager.getTexID(OCEAN));

  std::vector<ChunkStatus> initializer(NB_CHUNKS, NOT_GENERATED);
  _chunkStatus.resize(NB_CHUNKS, initializer);

  for (size_t i = 0; i < NB_CHUNKS; i++) {
    std::vector<std::unique_ptr<Chunk> > initializer2(NB_CHUNKS);
    _terrain.push_back(std::move(initializer2));
  }

  _contentGenerator.init();
  // _contentGenerator.saveToImage("contents");

  Camera& cam = Camera::getInstance();
  cam.setPointedPos(sf::Vector2f(CHUNK_BEGIN_X * CHUNK_SIZE + CHUNK_SIZE / 2,
                                 CHUNK_BEGIN_Y * CHUNK_SIZE + CHUNK_SIZE / 2));

  appendNewElements(_contentGenerator.genHerd(
                    sf::Vector2f(CHUNK_BEGIN_X * CHUNK_SIZE + CHUNK_SIZE / 2,
                                 CHUNK_BEGIN_Y * CHUNK_SIZE + CHUNK_SIZE / 2), 20));
}

void Game::generateChunk(size_t x, size_t y) {
  Chunk* newChunk = new Chunk(x, y, _terrainTexManager, _terrainGeometry);
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


          else if (_terrainGeometry.isOcean(tmp.x, tmp.y))
            _chunkStatus[tmp.x][tmp.y] = NOT_GENERATED;

          else
            generateChunk(tmp.x,tmp.y);
        }
      }
    }
  }

  _chunkStatus[x][y] = VISIBLE;
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
  if (_chunkStatus[camPosX][camPosY] == NOT_GENERATED)
    generateChunk(camPosX, camPosY);

  cam.setHeight( _terrain[camPosX][camPosY]->getHeight(cam.getPointedPos()));
  cam.apply();

  // Update terrains
  for (size_t i = 0; i < NB_CHUNKS; i++) {void setSubdivisionLevel(size_t newSubdLvl);
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

  glm::mat4 rotateElements = glm::rotate(glm::mat4(1.f),
                                         (float) M_PI / 180.f * cam.getTheta(),
                                         glm::vec3(0, 0, 1));

  rotateElements =           glm::rotate(rotateElements,
                                         ((float) M_PI / 180.f * cam.getPhi() - 90.f) / 2.f,
                                         glm::vec3(0, 1, 0));

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
      float baseHeight = _terrain[chunkPosX][chunkPosY]->getHeight(_igElements[i]->getPos());

      // Calculate new corners
      glm::vec3 corners3[4];
      float width = _igElements[i]->getSize().x;
      float height = _igElements[i]->getSize().y;

      corners3[0] = glm::vec3( 0,  width/2, height);
      corners3[1] = glm::vec3( 0, -width/2, height);
      corners3[2] = glm::vec3( 0, -width/2, 0);
      corners3[3] = glm::vec3( 0,  width/2, 0);

      glm::vec3 translatePos(_igElements[i]->getPos().x,
                             _igElements[i]->getPos().y,
                             baseHeight);

      glm::mat4 model = glm::translate(glm::mat4(1.f), translatePos) * rotateElements;

      // Compute their projections
      for (size_t i = 0; i < 4; i++) {
        glm::vec4 tmp(corners3[i], 1.f);
        tmp = model * tmp;
        tmp = cam.getViewProjectionMatrix() * tmp;
        corners3[i] = glm::vec3(tmp) / tmp.w;
      }

      // Culling
      if ((corners3[1].x > -1 && corners3[1].y > -1 &&
           corners3[1].x <  1 && corners3[1].y <  1) ||
          (corners3[3].x > -1 && corners3[3].y > -1 &&
           corners3[3].x <  1 && corners3[3].y <  1)) {

        std::array<float,12> vertices;

        for (size_t i = 0; i < 4; i++) {
          vertices[3*i]     = corners3[i].x;
          vertices[3*i + 1] = corners3[i].y;
          vertices[3*i + 2] = corners3[i].z;
        }

        _igElements[i]->setVertices(vertices);
        _igElements[i]->setVisible(true);
      }

      else
        _igElements[i]->setVisible(false);
    }

    else
      _igElements[i]->setVisible(false);
  }

  // Fill and sort the visible elements
  _visibleElmts.clear();

  for (size_t i = 0; i < _igElements.size(); i++) {
    if (_igElements[i]->isVisible())
      _visibleElmts.push_back(_igElements[i].get());
  }

  std::sort(_visibleElmts.begin(), _visibleElmts.end(), compDepthObj);
  _igElementDisplay.loadElements(_visibleElmts);
}

void Game::render() const {
  size_t nbTriangles = 0;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  Camera& cam = Camera::getInstance();
  glm::mat4 MVP = cam.getViewProjectionMatrix();

  glUseProgram(_terrainShader.getProgramID());
	glUniformMatrix4fv(glGetUniformLocation(_terrainShader.getProgramID(), "MVP"),
    1, GL_FALSE, &MVP[0][0]);

  // Background Ocean

  glDisable(GL_DEPTH_TEST);
  _ocean.draw();
  nbTriangles += 2;
  glEnable(GL_DEPTH_TEST);

  // Chunk

  size_t subdivLvl = 0;

  if (_wireframe)
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

  for (size_t i = 0; i < NB_CHUNKS; i++) {
    for (size_t j = 0; j < NB_CHUNKS; j++) {
      if (_chunkStatus[i][j] == VISIBLE) {
        nbTriangles += _terrain[i][j]->draw();
        subdivLvl = _terrain[i][j]->getSubdivisionLevel();
      }
    }
  }

  if (_wireframe)
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

  glUseProgram(0);

  // igElements

  glUseProgram(_igEShader.getProgramID());

  glDepthMask(false);

  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  _igElementDisplay.drawElements();

  glDisable(GL_BLEND);

  glDepthMask(true);

  glUseProgram(0);

  LogText& logText = LogText::getInstance();

  std::ostringstream renderStats;
  renderStats << "Triangles: " << nbTriangles << std::endl
              << "Elements:  " << _visibleElmts.size() << std::endl
              << "Subdivision level: " << subdivLvl << std::endl;

  logText.addLine(renderStats.str());
}

void Game::select(sf::IntRect rect, bool add) {
  if (!add)
    _selectedElmts.clear();

  for (unsigned int i = 0 ; i < _igElements.size() ; i++) {
    Controllable *ctrl = dynamic_cast<Controllable*>(_igElements[i].get());
    if (ctrl) {
      if (_selectedElmts.find(ctrl) == _selectedElmts.end()) { // _igElements[i] is not selected yet, we can bother to calculate
        sf::IntRect c = ctrl->getScreenCoord();

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

void Game::changeSubdivisionLevel(int increment) {
  for (size_t i = 0; i < NB_CHUNKS; i++) {
    for (size_t j = 0; j < NB_CHUNKS; j++) {
      if (_chunkStatus[i][j] != NOT_GENERATED) {
        int newLevel = _terrain[i][j]->getSubdivisionLevel() + increment;
        if (newLevel < 0)
          newLevel = 0;

        if (newLevel > _terrainGeometry.getNbSubdivLevels() - 1)
          _terrainGeometry.generateNewSubdivisionLevel();

        _terrain[i][j]->setSubdivisionLevel(newLevel);
      }
    }
  }
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
