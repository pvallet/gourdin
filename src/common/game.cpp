#include "game.h"

#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"

#include <ctime>

#define CHUNK_BEGIN_X 14
#define CHUNK_BEGIN_Y 16

Game::Game() :
  _wireframe(false),
  _terrainShader("src/shaders/heightmap.vert", "src/shaders/heightmap.frag"),
  _igEShader ("src/shaders/igElement.vert", "src/shaders/igElement.frag"),
  _skyboxShader ("src/shaders/skybox.vert", "src/shaders/skybox.frag"),
  _contentGenerator(_terrainGeometry),
  _ocean(2),
  _terrainGeometry(_reliefGenerator),
  _reliefGenerator(3, 0.06, 0.75) {}

void Game::resetCamera() {
  Camera& cam = Camera::getInstance();
  cam.setPointedPos(sf::Vector2f(CHUNK_BEGIN_X * CHUNK_SIZE + CHUNK_SIZE / 2,
                                 CHUNK_BEGIN_Y * CHUNK_SIZE + CHUNK_SIZE / 2));

  cam.setValues(INIT_R, INIT_THETA, INIT_PHI);
}

void Game::init() {
  srand(time(NULL));

  _terrainShader.load();
  _igEShader.load();
  _skyboxShader.load();

  glUseProgram(_igEShader.getProgramID());
  glUniform1f(glGetUniformLocation(_igEShader.getProgramID(), "elementNearPlane"), ELEMENT_NEAR_PLANE);
  glUseProgram(0);

  _terrainTexManager.loadFolder(BIOME_NB_ITEMS, "res/terrain/");
  _map.load("res/map/");
  _map.feedGeometryData(_terrainGeometry);

  _ocean.setTexIndex(_terrainTexManager.getTexID(OCEAN));
  _skybox.load("res/skybox/");

  std::vector<ChunkStatus> initializer(NB_CHUNKS, NOT_GENERATED);
  _chunkStatus.resize(NB_CHUNKS, initializer);

  for (size_t i = 0; i < NB_CHUNKS; i++) {
    std::vector<std::unique_ptr<Chunk> > initializer2(NB_CHUNKS);
    _terrain.push_back(std::move(initializer2));
  }

  _igElementDisplay.init();
  _contentGenerator.init();

  resetCamera();

  appendNewElements(_contentGenerator.genHerd(
                    sf::Vector2f(CHUNK_BEGIN_X * CHUNK_SIZE + CHUNK_SIZE / 2,
                                 CHUNK_BEGIN_Y * CHUNK_SIZE + CHUNK_SIZE / 2), 20));
}

void Game::generateChunk(size_t x, size_t y) {
  Chunk* newChunk = new Chunk(x, y, _terrainTexManager, _terrainGeometry);
  newChunk->generate();
  _terrain[x][y] = std::unique_ptr<Chunk>(newChunk);
  _chunkStatus[x][y] = EDGE;

  std::vector<igElement*> newTrees = _contentGenerator.genForestsInChunk(x, y);
  _terrain[x][y]->setTrees(newTrees);
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

void Game::appendNewElements(std::vector<igMovingElement*> elems) {

  for (size_t i = 0; i < elems.size(); i++)
    _igMovingElements.push_back(std::unique_ptr<igMovingElement>(elems[i]));

  for (size_t i = 0; i < elems.size(); i++) {
    if (!elems[i]->isDead())
      _activeElements.insert(elems[i]);
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

  for (auto it = _activeElements.begin(); it != _activeElements.end(); it++) {
    if ((*it)->isDead())
      toDelete.push_back(*it);
  }
  for (size_t i = 0; i < toDelete.size(); i++) {
    _activeElements.erase(toDelete[i]);
  }

  // Compute moving elements interactions
  for (auto it = _activeElements.begin(); it != _activeElements.end(); it++) {
    Antilope* atlp;
    Lion* lion;

    if (atlp = dynamic_cast<Antilope*>(*it))
      atlp->updateState(_activeElements);

    else if (lion = dynamic_cast<Lion*>(*it))
      lion->kill(_activeElements);
  }
}

void Game::compute2DCorners() {
  Camera& cam = Camera::getInstance();
  glm::mat4 rotateElements = glm::rotate(glm::mat4(1.f),
                                         (float) M_PI / 180.f * cam.getTheta(),
                                         glm::vec3(0, 0, 1));

  rotateElements =           glm::rotate(rotateElements,
                                         ((float) M_PI / 180.f * cam.getPhi() - 90.f) / 2.f,
                                         glm::vec3(0, 1, 0));

  for (auto it = _activeElements.begin(); it != _activeElements.end(); it++) {
    Controllable* ctrl;
    if (ctrl = dynamic_cast<Controllable*>(*it)) {
      // Calculate new corners
      glm::vec3 corners3[4];
      float width = ctrl->getSize().x;
      float height = ctrl->getSize().y;

      corners3[0] = glm::vec3( 0,  width/2, height);
      corners3[1] = glm::vec3( 0, -width/2, height);
      corners3[2] = glm::vec3( 0, -width/2, 0);
      corners3[3] = glm::vec3( 0,  width/2, 0);

      glm::vec3 translatePos(ctrl->getPos().x,
                             ctrl->getPos().y,
                             ctrl->getHeight());

      glm::mat4 model = glm::translate(glm::mat4(1.f), translatePos) * rotateElements;

      // We keep track of the normalization factor to get the absolute z screen value,
      // in order to remove nearest igElements
      float w3;

      // Compute their projections
      for (size_t i = 0; i < 4; i++) {
        glm::vec4 tmp(corners3[i], 1.f);
        tmp = model * tmp;
        tmp = cam.getViewProjectionMatrix() * tmp;
        corners3[i] = glm::vec3(tmp) / tmp.w;
        w3 = tmp.w;
      }

      std::array<float,12> vertices;

      if (corners3[3].z * w3 > ELEMENT_NEAR_PLANE) {
        for (size_t i = 0; i < 4; i++) {
          vertices[3*i]     = corners3[i].x;
          vertices[3*i + 1] = corners3[i].y;
          vertices[3*i + 2] = corners3[i].z;
        }
      }

      else {
        for (size_t i = 0; i < 12; i++) {
          vertices[i] = 0;
        }
      }

      ctrl->setProjectedVertices(vertices);
    }
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

      if (_chunkStatus[i][j] == VISIBLE)
        _terrain[i][j]->computeSubdivisionLevel();
    }
  }

  LogText& logText = LogText::getInstance();
  std::ostringstream subdivLvl;
  subdivLvl << "Current subdivision level: " << _terrain[camPosX][camPosY]->getSubdivisionLevel() << std::endl;
  logText.addLine(subdivLvl.str());

  updateMovingElementsStates();
  // Update positions of igMovingElement regardless of them being visible
  for (auto it = _igMovingElements.begin(); it != _igMovingElements.end(); it++) {
    (*it)->update(elapsed);
  }

   // Fill the visible elements
   std::vector<igElement*> visibleElmts;

  for (auto it = _igMovingElements.begin(); it != _igMovingElements.end(); it++) {
    int chunkPosX = (*it)->getPos().x / CHUNK_SIZE;
    int chunkPosY = (*it)->getPos().y / CHUNK_SIZE;

    if (chunkPosX == NB_CHUNKS)
      chunkPosX--;
    if (chunkPosY == NB_CHUNKS)
      chunkPosY--;

    if (_chunkStatus[chunkPosX][chunkPosY] == VISIBLE) {
      // No test yet to see if the element can move to its new pos (no collision)
      float height = _terrain[chunkPosX][chunkPosY]->getHeight((*it)->getPos());

      (*it)->setHeight(height);
      (*it)->updateDisplay(elapsed, cam.getTheta());

      visibleElmts.push_back(it->get());
    }
  }

  _igElementDisplay.loadElements(visibleElmts);

  compute2DCorners();
}

void Game::renderLifeBars(sf::RenderWindow& window) const {
  sf::RectangleShape lifeBar(sf::Vector2f(20.f, 2.f));
  lifeBar.setFillColor(sf::Color::Green);

  sf::RectangleShape fullLifeBar(sf::Vector2f(20.f, 2.f));
  fullLifeBar.setFillColor(sf::Color::Transparent);
  fullLifeBar.setOutlineColor(sf::Color::Black);
  fullLifeBar.setOutlineThickness(1);

  sf::IntRect corners;
  float maxHeightFactor;

  for(auto it = _selectedElmts.begin(); it != _selectedElmts.end(); ++it) {
    Lion* lion;
    if (lion = dynamic_cast<Lion*>(*it)) {
      corners = (*it)->getScreenCoord();
      maxHeightFactor = (*it)->getMaxHeightFactor(); // The lifeBar must not change when switching animations

        lifeBar.setSize(sf::Vector2f(20.f* lion->getStamina() / 100.f, 2.f));

      lifeBar.setPosition(corners.left + corners.width/2 - 10,
        corners.top - corners.height*maxHeightFactor + corners.height - 5);
      fullLifeBar.setPosition(corners.left + corners.width/2 - 10,
        corners.top - corners.height*maxHeightFactor + corners.height - 5);

      window.draw(lifeBar);
      window.draw(fullLifeBar);
      lifeBar.setSize(sf::Vector2f(20.f, 2.f));
    }
  }
}

void Game::render() const {
  size_t nbTriangles = 0;
  size_t nbElements = 0;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  Camera& cam = Camera::getInstance();
  glm::mat4 MVP = cam.getSkyboxViewProjectionMatrix();

  // Skybox

  glUseProgram(_skyboxShader.getProgramID());
	glUniformMatrix4fv(glGetUniformLocation(_skyboxShader.getProgramID(), "MVP"),
    1, GL_FALSE, &MVP[0][0]);

  glDisable(GL_DEPTH_TEST);
  _skybox.draw();
  nbTriangles += 12;
  glEnable(GL_DEPTH_TEST);

  glUseProgram(0);

  // Terrain draws

  MVP = cam.getViewProjectionMatrix();

  glUseProgram(_terrainShader.getProgramID());
	glUniformMatrix4fv(glGetUniformLocation(_terrainShader.getProgramID(), "MVP"),
    1, GL_FALSE, &MVP[0][0]);

  // Background Ocean

  glDisable(GL_DEPTH_TEST);
  _ocean.draw();
  nbTriangles += 2;
  glEnable(GL_DEPTH_TEST);

  // Chunk

  if (_wireframe)
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

  for (size_t i = 0; i < NB_CHUNKS; i++) {
    for (size_t j = 0; j < NB_CHUNKS; j++) {
      if (_chunkStatus[i][j] == VISIBLE)
        nbTriangles += _terrain[i][j]->draw();
    }
  }

  if (_wireframe)
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

  glUseProgram(0);

  // igElements

  glm::mat4 rotateElements = glm::rotate(glm::mat4(1.f),
                                         (float) M_PI / 180.f * cam.getTheta(),
                                         glm::vec3(0, 0, 1));

  rotateElements =           glm::rotate(rotateElements,
                                         ((float) M_PI / 180.f * cam.getPhi() - 90.f) / 2.f,
                                         glm::vec3(0, 1, 0));

  glUseProgram(_igEShader.getProgramID());
  glUniformMatrix4fv(glGetUniformLocation(_igEShader.getProgramID(), "VP"),
    1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(glGetUniformLocation(_igEShader.getProgramID(), "MODEL"),
    1, GL_FALSE, &rotateElements[0][0]);

  // Two passes to avoid artifacts due to alpha blending

  glUniform1i(glGetUniformLocation(_igEShader.getProgramID(), "onlyOpaqueParts"), true);
  nbElements += _igElementDisplay.drawElements();

  for (size_t i = 0; i < NB_CHUNKS; i++) {
    for (size_t j = 0; j < NB_CHUNKS; j++) {
      if (_chunkStatus[i][j] == VISIBLE)
        _terrain[i][j]->drawTrees();
    }
  }

  glUniform1i(glGetUniformLocation(_igEShader.getProgramID(), "onlyOpaqueParts"), false);

  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  _igElementDisplay.drawElements();

  for (size_t i = 0; i < NB_CHUNKS; i++) {
    for (size_t j = 0; j < NB_CHUNKS; j++) {
      if (_chunkStatus[i][j] == VISIBLE)
        nbElements += _terrain[i][j]->drawTrees();
    }
  }

  glDisable(GL_BLEND);

  glUseProgram(0);

  LogText& logText = LogText::getInstance();

  std::ostringstream renderStats;
  renderStats << "Triangles: " << nbTriangles << std::endl
              << "Elements:  " << nbElements << std::endl;

  logText.addLine(renderStats.str());
}

void Game::select(sf::IntRect rect, bool add) {
  if (!add)
    _selectedElmts.clear();

  for (auto it = _igMovingElements.begin(); it != _igMovingElements.end(); it++) {
    Controllable *ctrl = dynamic_cast<Controllable*>(it->get());
    if (ctrl && !ctrl->isDead()) {
      // _igMovingElements[i] is not selected yet, we can bother to calculate
      if (_selectedElmts.find(ctrl) == _selectedElmts.end()) {
        sf::IntRect SpriteRect = ctrl->getScreenCoord();

        int centerX, centerY;

        centerX = SpriteRect.left + SpriteRect.width / 2;
        centerY = SpriteRect.top + SpriteRect.height / 2;

        if (rect.contains(centerX, centerY))
          _selectedElmts.insert(ctrl);

        else if (   SpriteRect.contains(rect.left, rect.top) ||
                    SpriteRect.contains(rect.left + rect.width, rect.top) ||
                    SpriteRect.contains(rect.left + rect.width, rect.top + rect.height) ||
                    SpriteRect.contains(rect.left, rect.top + rect.height)  ) {
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

Human* Game::moveCharacter(sf::Vector2i screenTarget, Human* focusedCharacter) {
  for (size_t i = 0; i < _tribe.size(); i++) {
    sf::IntRect spriteRect = _tribe[i]->getScreenCoord();

    if (spriteRect.contains(screenTarget))
      return _tribe[i];
  }

  focusedCharacter->setTarget(get2DCoord(screenTarget));
  return focusedCharacter;
}

void Game::addLion(sf::Vector2i screenTarget) {
  appendNewElements(_contentGenerator.genLion(get2DCoord(screenTarget)));
}

void Game::genTribe(sf::Vector2f pos) {
  std::vector<igMovingElement*> tribe = _contentGenerator.genTribe(pos);
  appendNewElements(tribe);
  for (size_t i = 0; i < tribe.size(); i++) {
    _tribe.push_back(dynamic_cast<Human*>(tribe[i]));
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
