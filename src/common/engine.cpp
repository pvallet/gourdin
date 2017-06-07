#include "engine.h"

#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"
#include "reliefGenerator.h"

#include <ctime>

#define CHUNK_BEGIN_X 14
#define CHUNK_BEGIN_Y 20

Engine::Engine() :
  _wireframe(false),
  _contentGenerator(_terrainGeometry),
  _ocean(2),
  _mapInfoExtractor(_terrainGeometry),
  _reliefGenerator(_mapInfoExtractor),
  _terrainShader("src/shaders/heightmap.vert", "src/shaders/heightmap.frag"),
  _igEShader ("src/shaders/igElement.vert", "src/shaders/igElement.frag"),
  _skyboxShader ("src/shaders/skybox.vert", "src/shaders/skybox.frag") {}

void Engine::resetCamera() {
  Camera& cam = Camera::getInstance();
  cam.setPointedPos(sf::Vector2f(CHUNK_BEGIN_X * CHUNK_SIZE + CHUNK_SIZE / 2,
                                 CHUNK_BEGIN_Y * CHUNK_SIZE + CHUNK_SIZE / 2));

  cam.setValues(INIT_R, INIT_THETA, INIT_PHI);
}

void Engine::init() {
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

  GeneratedImage relief;

  if (relief.loadFromFile("res/map/relief.png"))
    _terrainGeometry.setReliefGenerator(relief);

  else {
    std::cout << "Generating relief mask" << '\n';

    _mapInfoExtractor.convertMapData(512);
    _mapInfoExtractor.generateBiomesTransitions(7);

    _reliefGenerator.generateRelief();
    _reliefGenerator.saveToFile("res/map/relief.png");
    _terrainGeometry.setReliefGenerator(_reliefGenerator.getRelief());
    std::cout << "Done Generating relief mask" << '\n';
  }

  // The base subdivision level is 1, it will take into account the generated relief contrary to level 0
  _terrainGeometry.generateNewSubdivisionLevel();

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
                                 CHUNK_BEGIN_Y * CHUNK_SIZE + CHUNK_SIZE / 2), 20, DEER));

  appendNewElements(_contentGenerator.genHerds());
}

void Engine::generateChunk(size_t x, size_t y) {
  Chunk* newChunk = new Chunk(x, y, _terrainTexManager, _terrainGeometry);
  newChunk->generate();
  _terrain[x][y] = std::unique_ptr<Chunk>(newChunk);
  _chunkStatus[x][y] = EDGE;

  std::vector<igElement*> newTrees = _contentGenerator.genForestsInChunk(x, y);
  _terrain[x][y]->setTrees(newTrees);
}

sf::Vector2i Engine::neighbour(size_t x, size_t y, size_t index) const {
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
    default:
      std::cerr << "Error in Engine::neighbour: Index out of bounds" << '\n';
      return sf::Vector2i(x,y);
      break;
  }
}

void Engine::generateNeighbourChunks(size_t x, size_t y) {
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

void Engine::appendNewElements(std::vector<igMovingElement*> elems) {

  for (size_t i = 0; i < elems.size(); i++)
    _igMovingElements.push_back(std::unique_ptr<igMovingElement>(elems[i]));

  for (size_t i = 0; i < elems.size(); i++) {
    Controllable* ctrl = dynamic_cast<Controllable*>(elems[i]);
    if (ctrl)
      _controllableElements.insert(ctrl);
  }
}

void Engine::updateMovingElementsStates(const std::vector<std::list<igMovingElement*> >& sortedElements) {
  for (int i = 0; i < NB_CHUNKS; i++) {
    for (int j = 0; j < NB_CHUNKS; j++) {
      if (!sortedElements[i*NB_CHUNKS + j].empty()) {
        std::list<igMovingElement*> elmtsInSurroundingChunks;

        for (int k = std::max(0, i-1); k < std::min(NB_CHUNKS-1, i+1); k++) {
        for (int l = std::max(0, j-1); l < std::min(NB_CHUNKS-1, j+1); l++) {
          elmtsInSurroundingChunks.insert(elmtsInSurroundingChunks.end(),
            sortedElements[k*NB_CHUNKS + l].begin(), sortedElements[k*NB_CHUNKS + l].end());
        }
        }

        for (auto it = sortedElements[i*NB_CHUNKS + j].begin(); it != sortedElements[i*NB_CHUNKS + j].end(); it++) {
          (*it)->updateState(elmtsInSurroundingChunks);
        }
      }
    }
  }
}

void Engine::compute2DCorners() {
  Camera& cam = Camera::getInstance();
  glm::mat4 rotateElements = glm::rotate(glm::mat4(1.f),
                                         (float) M_PI / 180.f * cam.getTheta(),
                                         glm::vec3(0, 0, 1));

  rotateElements =           glm::rotate(rotateElements,
                                         ((float) M_PI / 180.f * cam.getPhi() - 90.f) / 2.f,
                                         glm::vec3(0, 1, 0));

  for (auto it = _controllableElements.begin(); it != _controllableElements.end(); it++) {
    sf::Vector3f pos;
    pos.x = (*it)->getPos().x;
    pos.y = (*it)->getPos().y;
    pos.z = (*it)->getHeight();

    std::array<float,12> vertices;

    if (!(*it)->isDead() && vu::norm(pos-cam.getPos()) > ELEMENT_NEAR_PLANE) {
      // Calculate new corners
      glm::vec3 corners3[4];
      float width = (*it)->getSize().x;
      float height = (*it)->getSize().y;

      corners3[0] = glm::vec3( 0,  width/2, height);
      corners3[1] = glm::vec3( 0, -width/2, height);
      corners3[2] = glm::vec3( 0, -width/2, 0);
      corners3[3] = glm::vec3( 0,  width/2, 0);

      glm::vec3 translatePos((*it)->getPos().x,
                             (*it)->getPos().y,
                             (*it)->getHeight());

      glm::mat4 model = glm::translate(glm::mat4(1.f), translatePos) * rotateElements;

      // Compute their projections
      for (size_t i = 0; i < 4; i++) {
        glm::vec4 tmp(corners3[i], 1.f);
        tmp = model * tmp;
        tmp = cam.getViewProjectionMatrix() * tmp;
        corners3[i] = glm::vec3(tmp) / tmp.w;
      }

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

    (*it)->setProjectedVertices(vertices);
  }
}

void Engine::update(sf::Time elapsed) {
  Camera& cam = Camera::getInstance();
  sf::Vector2u camPos = convertToChunkCoords(cam.getPointedPos());

  // Update camera
  if (_chunkStatus[camPos.x][camPos.y] == NOT_GENERATED)
    generateChunk(camPos.x, camPos.y);

  cam.setHeight( _terrain[camPos.x][camPos.y]->getHeight(cam.getPointedPos()));
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
  subdivLvl << "Current subdivision level: " << _terrain[camPos.x][camPos.y]->getSubdivisionLevel() << std::endl;
  logText.addLine(subdivLvl.str());

  // Update positions of igMovingElement regardless of them being visible
  for (auto it = _igMovingElements.begin(); it != _igMovingElements.end(); it++) {
    (*it)->update(elapsed);
  }

  // Fill the visible elements
  std::vector<igElement*> visibleElmts;
  // sortedElements contains a list of the moving elements in the chunk (x,y) in index x * NB_CHUNKS + y
  std::vector<std::list<igMovingElement*> > sortedElements(NB_CHUNKS*NB_CHUNKS);

  for (auto it = _igMovingElements.begin(); it != _igMovingElements.end(); it++) {
    sf::Vector2u chunkPos = convertToChunkCoords((*it)->getPos());

    if (!(*it)->isDead())
      sortedElements[chunkPos.x * NB_CHUNKS + chunkPos.y].push_back(it->get());
  }

  updateMovingElementsStates(sortedElements);

  // Update graphics of visible elements
  for (auto it = _igMovingElements.begin(); it != _igMovingElements.end(); it++) {
    sf::Vector2u chunkPos = convertToChunkCoords((*it)->getPos());

    if (_chunkStatus[chunkPos.x][chunkPos.y] == VISIBLE) {
      // No test yet to see if the element can move to its new pos (no collision)
      float height = _terrain[chunkPos.x][chunkPos.y]->getHeight((*it)->getPos());

      (*it)->setHeight(height);
      (*it)->updateDisplay(elapsed, cam.getTheta());

      visibleElmts.push_back(it->get());
    }
  }

  _igElementDisplay.loadElements(visibleElmts);

  // Remove the dead elements from the selection and the controllable elements
  std::vector<igMovingElement*> toDelete;
  for (auto it = _controllableElements.begin(); it != _controllableElements.end(); it++) {
   if ((*it)->isDead())
     toDelete.push_back(*it);
  }
  for (size_t i = 0; i < toDelete.size(); i++) {
   _controllableElements.erase((Controllable*) toDelete[i]);
   _selectedElmts.erase((Controllable*) toDelete[i]);
  }

  compute2DCorners();
}

void Engine::renderLifeBars(sf::RenderWindow& window) const {
  sf::RectangleShape lifeBar(sf::Vector2f(20.f, 2.f));
  lifeBar.setFillColor(sf::Color::Green);

  sf::RectangleShape fullLifeBar(sf::Vector2f(20.f, 2.f));
  fullLifeBar.setFillColor(sf::Color::Transparent);
  fullLifeBar.setOutlineColor(sf::Color::Black);
  fullLifeBar.setOutlineThickness(1);

  sf::IntRect corners;
  float maxHeightFactor;

  for(auto it = _selectedElmts.begin(); it != _selectedElmts.end(); ++it) {
    Lion* lion = dynamic_cast<Lion*>(*it);
    if (lion) {
      corners = (*it)->getScreenCoord();
      // Otherwise the selected element is outside the screen
      if (corners.width != 0) {
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
}

void Engine::render() const {
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

  glm::vec3 camPos = vu::convertGLM(cam.getPos());

  glUseProgram(_igEShader.getProgramID());
  glUniformMatrix4fv(glGetUniformLocation(_igEShader.getProgramID(), "VP"),
    1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(glGetUniformLocation(_igEShader.getProgramID(), "MODEL"),
    1, GL_FALSE, &rotateElements[0][0]);
  glUniform3fv(glGetUniformLocation(_igEShader.getProgramID(), "camPos"),
    1, &camPos[0]);

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

void Engine::select(sf::IntRect rect, bool add) {
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

void Engine::moveSelection(sf::Vector2i screenTarget) {
  sf::Vector2f target = get2DCoord(screenTarget);

  for(auto it = _selectedElmts.begin(); it != _selectedElmts.end(); ++it) {
    Controllable* ctrl = dynamic_cast<Controllable*>(*it);
    if (ctrl) {
      ctrl->setTarget(target);
    }
  }
}

void Engine::moveCamera(sf::Vector2f newAimedPos) {
  generateChunk(newAimedPos.x / CHUNK_SIZE, newAimedPos.y / CHUNK_SIZE);
  Camera& cam = Camera::getInstance();
  cam.setPointedPos(newAimedPos);
}

Controllable* Engine::moveCharacter(sf::Vector2i screenTarget, Controllable* focusedCharacter) {
  for (size_t i = 0; i < _tribe.size(); i++) {
    sf::IntRect spriteRect = _tribe[i]->getScreenCoord();

    if (spriteRect.contains(screenTarget))
      return _tribe[i];
  }

  focusedCharacter->setTarget(get2DCoord(screenTarget));
  return focusedCharacter;
}

void Engine::addLion(sf::Vector2i screenTarget) {
  appendNewElements(_contentGenerator.genLion(get2DCoord(screenTarget)));
}

void Engine::genTribe(sf::Vector2f pos) {
  std::vector<igMovingElement*> tribe = _contentGenerator.genTribe(pos);
  appendNewElements(tribe);
  for (size_t i = 0; i < tribe.size(); i++) {
    _tribe.push_back(dynamic_cast<Controllable*>(tribe[i]));
  }
}

sf::Vector2f Engine::get2DCoord(sf::Vector2i screenTarget) const {
  Camera& cam = Camera::getInstance();
  screenTarget.y = cam.getH() - screenTarget.y; // Inverted coordinates

  GLfloat d;
 glReadPixels(screenTarget.x, screenTarget.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &d);

  glm::vec3 modelCoord = glm::unProject(glm::vec3(screenTarget.x, screenTarget.y,d),
    glm::mat4(1.f), cam.getViewProjectionMatrix(),
    glm::vec4(0,0, cam.getW(), cam.getH()));

  return sf::Vector2f( modelCoord.x, modelCoord.y);
}

sf::Vector3f Engine::getNormalOnCameraPointedPos() const {
  Camera& cam = Camera::getInstance();
  sf::Vector2u chunkPos = convertToChunkCoords(cam.getPointedPos());

  return _terrain[chunkPos.x][chunkPos.y]->getNorm(cam.getPointedPos());
}