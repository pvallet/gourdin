#include "engine.h"

#include "opengl.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "camera.h"
#include "log.h"
#include "reliefGenerator.h"

#include <ctime>

Engine::Engine() :
  _wireframe(false),
  _contentGenerator(_terrainGeometry),
  _ocean(2),
  _mapInfoExtractor(_terrainGeometry),
  _reliefGenerator(_mapInfoExtractor),
  _terrain(NB_CHUNKS*NB_CHUNKS) {}

void Engine::init() {
  Clock initTimer;
  srand(time(NULL));

  int previousTime = initTimer.getElapsedTime();

  _terrainShader.load("src/shaders/heightmap.vert", "src/shaders/heightmap.frag");
  _igEShader.load("src/shaders/igElement.vert", "src/shaders/igElement.frag");
  _skyboxShader.load("src/shaders/skybox.vert", "src/shaders/skybox.frag");
  _depthInColorBufferShader.load("src/shaders/2D_shaders/2D.vert", "src/shaders/2D_shaders/depthToColor.frag");

  _igEShader.bind();
  glUniform1f(_igEShader.getUniformLocation("elementNearPlane"), ELEMENT_NEAR_PLANE);
  Shader::unbind();

  SDL_Log("Loading shaders: %d ms", initTimer.getElapsedTime() - previousTime);
  previousTime = initTimer.getElapsedTime();

  _terrainTexManager.loadFolder(BIOME_NB_ITEMS, "res/terrain/");
  _map.load("res/map/");
  _map.feedGeometryData(_terrainGeometry);

  SDL_Log("Loading terrain data: %d ms", initTimer.getElapsedTime() - previousTime);
  previousTime = initTimer.getElapsedTime();

  GeneratedImage relief;

  if (relief.loadFromFile("res/map/relief.png"))
    _terrainGeometry.setReliefGenerator(relief);

  else {
    SDL_Log("Generating relief mask");

    _mapInfoExtractor.convertMapData(512);
    _mapInfoExtractor.generateBiomesTransitions(7);

    _reliefGenerator.generateRelief();
    _reliefGenerator.saveToFile("res/map/relief.png");
    _terrainGeometry.setReliefGenerator(_reliefGenerator.getRelief());
    SDL_Log("Done Generating relief mask");
  }

  // The base subdivision level is 1, it will take into account the generated relief contrary to level 0
  _terrainGeometry.generateNewSubdivisionLevel();

  SDL_Log("Generate geometry: %d ms", initTimer.getElapsedTime() - previousTime);
  previousTime = initTimer.getElapsedTime();

  _ocean.setTexture(_terrainTexManager.getTexture(OCEAN));
  _skybox.load("res/skybox/");

  Camera& cam = Camera::getInstance();
  _globalFBO.init(cam.getW(), cam.getH(), GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
  _depthInColorBufferFBO.init(cam.getW(), cam.getH(), GL_R32F, GL_RED, GL_FLOAT);
  _depthTexturedRectangle.reset(new TexturedRectangle(_globalFBO.getDepthTexture(), -1, -1, 2, 2));

  SDL_Log("Camera skybox and ocean: %d ms", initTimer.getElapsedTime() - previousTime);
  previousTime = initTimer.getElapsedTime();

  _igElementDisplay.init();
  _contentGenerator.init();

  SDL_Log("Init igElementDisplay and ContentGenerator: %d ms", initTimer.getElapsedTime() - previousTime);
  previousTime = initTimer.getElapsedTime();

  std::vector<Chunk*> newChunks;
  for (size_t i = 0; i < NB_CHUNKS; i++) {
    for (size_t j = 0; j < NB_CHUNKS; j++) {
      newChunks.push_back(new Chunk(i, j, _terrainTexManager, _terrainGeometry));
    }
  }

  #pragma omp parallel for
  for (size_t i = 0; i < NB_CHUNKS*NB_CHUNKS; i++) {
    size_t x = i / NB_CHUNKS;
    size_t y = i - x * NB_CHUNKS;
    std::vector<igElement*> newTrees = _contentGenerator.genForestsInChunk(x,y);
    newChunks[x*NB_CHUNKS + y]->setTrees(newTrees);
  }

  for (size_t i = 0; i < NB_CHUNKS; i++) {
    for (size_t j = 0; j < NB_CHUNKS; j++) {
      newChunks[i*NB_CHUNKS + j]->generate();
      newChunks[i*NB_CHUNKS + j]->loadTrees();
      _terrain[i*NB_CHUNKS + j] = std::unique_ptr<Chunk>(newChunks[i*NB_CHUNKS + j]);
    }
  }

  SDL_Log("Generating chunks: %d ms", initTimer.getElapsedTime() - previousTime);
  previousTime = initTimer.getElapsedTime();

  appendNewElements(_contentGenerator.genHerd(cam.getPointedPos(), 20, DEER));
  appendNewElements(_contentGenerator.genHerds());

  SDL_Log("Generating herds: %d ms", initTimer.getElapsedTime() - previousTime);
  previousTime = initTimer.getElapsedTime();

  SDL_Log("Initialization time: %d ms", initTimer.getElapsedTime());
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
    glm::vec3 pos;
    pos.x = (*it)->getPos().x;
    pos.y = (*it)->getPos().y;
    pos.z = (*it)->getHeight();

    std::array<float,12> vertices;

    if (!(*it)->isDead() && glm::length(pos-cam.getPos()) > ELEMENT_NEAR_PLANE) {
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

void Engine::updateCulling() {
  Camera& cam = Camera::getInstance();

  // Compute frustum planes

  float theta = cam.getTheta();
  float phi   = cam.getPhi();
  float alpha = cam.getFov() * cam.getRatio() / 2.f;

  std::vector<glm::vec3> camFrustumPlaneNormals;
  // Bottom of the view
  camFrustumPlaneNormals.push_back(ut::carthesian(1.f, theta, phi + 90.f - cam.getFov() / 2.f));
  // Top
  camFrustumPlaneNormals.push_back(ut::carthesian(1.f, theta, phi + 90.f + cam.getFov() / 2.f) * -1.f);
  // Right
  camFrustumPlaneNormals.push_back(glm::rotate(
    ut::carthesian(1.f, theta + 90.f, 90.f),
    (float) (- alpha*RAD), ut::carthesian(1.f, theta + 180.f, 90.f - phi)));
  // Left
  camFrustumPlaneNormals.push_back(glm::rotate(
    ut::carthesian(1.f, theta - 90.f, 90.f),
    (float) (alpha*RAD), ut::carthesian(1.f, theta + 180.f, 90.f - phi)));

  // Update terrains
  #pragma omp parallel for collapse(2)
  for (size_t i = 0; i < NB_CHUNKS; i++) {
    for (size_t j = 0; j < NB_CHUNKS; j++) {
      _terrain[i*NB_CHUNKS + j]->computeCulling(camFrustumPlaneNormals);

      if (_terrain[i*NB_CHUNKS + j]->isVisible())
        _terrain[i*NB_CHUNKS + j]->computeDistanceOptimizations();
    }
  }
}

void Engine::update(int msElapsed) {
  updateCulling();

  // Update positions of igMovingElement regardless of them being visible
  for (auto it = _igMovingElements.begin(); it != _igMovingElements.end(); it++) {
    (*it)->update(msElapsed);
  }

  // Fill the visible elements
  std::vector<igElement*> visibleElmts;
  // sortedElements contains a list of the moving elements in the chunk (x,y) in index x * NB_CHUNKS + y
  std::vector<std::list<igMovingElement*> > sortedElements(NB_CHUNKS*NB_CHUNKS);

  for (auto it = _igMovingElements.begin(); it != _igMovingElements.end(); it++) {
    glm::uvec2 chunkPos = ut::convertToChunkCoords((*it)->getPos());

    if (!(*it)->isDead())
      sortedElements[chunkPos.x * NB_CHUNKS + chunkPos.y].push_back(it->get());
  }

  updateMovingElementsStates(sortedElements);

  // Update graphics of visible elements
  Camera& cam = Camera::getInstance();

  for (auto it = _igMovingElements.begin(); it != _igMovingElements.end(); it++) {
    glm::uvec2 chunkPos = ut::convertToChunkCoords((*it)->getPos());

    if (_terrain[chunkPos.x*NB_CHUNKS + chunkPos.y]->isVisible() &&
        _terrain[chunkPos.x*NB_CHUNKS + chunkPos.y]->getDisplayElements()) {
      // No test yet to see if the element can move to its new pos (no collision)
      float height = _terrain[chunkPos.x*NB_CHUNKS + chunkPos.y]->getHeight((*it)->getPos());

      (*it)->setHeight(height);
      (*it)->updateDisplay(msElapsed, cam.getTheta());

      visibleElmts.push_back(it->get());
    }
  }

  _igElementDisplay.loadElements(visibleElmts);

  // Remove the dead elements from the controllable elements
  std::vector<Controllable*> toDelete;
  for (auto it = _controllableElements.begin(); it != _controllableElements.end(); it++) {
   if ((*it)->isDead())
     toDelete.push_back(*it);
  }
  for (size_t i = 0; i < toDelete.size(); i++) {
    _controllableElements.erase(toDelete[i]);
    _deadControllableElements.insert(toDelete[i]);
  }

  compute2DCorners();
}

void Engine::renderToFBO() const {
  size_t nbTriangles = 0;
  size_t nbElements = 0;

  _globalFBO.bind();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  Camera& cam = Camera::getInstance();
  glm::mat4 MVP = cam.getSkyboxViewProjectionMatrix();

  // Skybox

  _skyboxShader.bind();
	glUniformMatrix4fv(_skyboxShader.getUniformLocation("MVP"),
    1, GL_FALSE, &MVP[0][0]);

  glDisable(GL_DEPTH_TEST);
  _skybox.draw();
  nbTriangles += 12;
  glEnable(GL_DEPTH_TEST);

  Shader::unbind();

  // Terrain draws

  MVP = cam.getViewProjectionMatrix();

  _terrainShader.bind();
	glUniformMatrix4fv(_terrainShader.getUniformLocation("MVP"),
    1, GL_FALSE, &MVP[0][0]);

  // Background Ocean

  glDisable(GL_DEPTH_TEST);
  _ocean.draw();
  nbTriangles += 2;
  glEnable(GL_DEPTH_TEST);

  // Chunk

#ifndef __ANDROID__
  if (_wireframe)
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
#endif

  for (size_t i = 0; i < NB_CHUNKS; i++) {
    for (size_t j = 0; j < NB_CHUNKS; j++) {
      if (_terrain[i*NB_CHUNKS + j]->isVisible())
        nbTriangles += _terrain[i*NB_CHUNKS + j]->draw();
    }
  }

#ifndef __ANDROID__
  if (_wireframe)
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
#endif

  Shader::unbind();

  // igElements

  glm::mat4 rotateElements = glm::rotate(glm::mat4(1.f),
                                         (float) M_PI / 180.f * cam.getTheta(),
                                         glm::vec3(0, 0, 1));

  rotateElements =           glm::rotate(rotateElements,
                                         ((float) M_PI / 180.f * cam.getPhi() - 90.f) / 2.f,
                                         glm::vec3(0, 1, 0));

  _igEShader.bind();
  glUniformMatrix4fv(_igEShader.getUniformLocation("VP"),
    1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(_igEShader.getUniformLocation("MODEL"),
    1, GL_FALSE, &rotateElements[0][0]);
  glUniform3fv(_igEShader.getUniformLocation("camPos"),
    1, &cam.getPos()[0]);

  // Two passes to avoid artifacts due to alpha blending

  glUniform1i(_igEShader.getUniformLocation("onlyOpaqueParts"), true);
  nbElements += _igElementDisplay.drawElements();

  for (size_t i = 0; i < NB_CHUNKS; i++) {
    for (size_t j = 0; j < NB_CHUNKS; j++) {
      if (_terrain[i*NB_CHUNKS + j]->isVisible() &&
          _terrain[i*NB_CHUNKS + j]->getDisplayElements())
        _terrain[i*NB_CHUNKS + j]->drawTrees();
    }
  }

  glUniform1i(_igEShader.getUniformLocation("onlyOpaqueParts"), false);

  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  _igElementDisplay.drawElements();

  for (size_t i = 0; i < NB_CHUNKS; i++) {
    for (size_t j = 0; j < NB_CHUNKS; j++) {
      if (_terrain[i*NB_CHUNKS + j]->isVisible() &&
          _terrain[i*NB_CHUNKS + j]->getDisplayElements())
        nbElements += _terrain[i*NB_CHUNKS + j]->drawTrees();
    }
  }

  glDisable(GL_BLEND);

  Shader::unbind();
  FrameBufferObject::unbind();

  Log& logText = Log::getInstance();

  std::ostringstream renderStats;
  renderStats << "Triangles: " << nbTriangles << std::endl
              << "Elements:  " << nbElements << std::endl;

  logText.addLine(renderStats.str());
}

void Engine::addLion(glm::ivec2 screenTarget, float minDistToAntilopes) {
  glm::vec2 lionPos = get2DCoord(screenTarget);
  for (auto it = _igMovingElements.begin(); it != _igMovingElements.end(); it++) {
    Antilope* atlp = dynamic_cast<Antilope*>(it->get());
    if (atlp && !atlp->isDead() && glm::length(atlp->getPos() - lionPos) < minDistToAntilopes)
      throw std::runtime_error("Can't spawn a predator here, too close to its preys");
  }
  std::vector<igMovingElement*> newLion = _contentGenerator.genLion(lionPos);

  if (newLion.size() == 0)
    throw std::runtime_error("Can't spawn a predator on this biome");

  appendNewElements(newLion);
}

std::vector<Controllable*> Engine::genTribe(glm::vec2 pos) {
  std::vector<igMovingElement*> tribe = _contentGenerator.genTribe(pos);
  appendNewElements(tribe);

  std::vector<Controllable*> res;
  for (size_t i = 0; i < tribe.size(); i++) {
    res.push_back(dynamic_cast<Controllable*>(tribe[i]));
  }

  return res;
}

void Engine::deleteElements(const std::vector<igMovingElement*>& elementsToDelete) {
  for (auto elmt = _igMovingElements.begin(); elmt != _igMovingElements.end(); ) {
    bool hasDeleted = false;
    for (auto toDel = elementsToDelete.begin(); toDel != elementsToDelete.end(); toDel++) {
      if ((*toDel) == elmt->get()) {
        elmt = _igMovingElements.erase(elmt);
        hasDeleted = true;
      }
    }

    if (!hasDeleted)
      elmt++;
  }

  for (auto elmt = _controllableElements.begin(); elmt != _controllableElements.end(); ) {
    bool hasDeleted = false;
    for (auto toDel = elementsToDelete.begin(); toDel != elementsToDelete.end(); toDel++) {
      if (*toDel == *elmt) {
        elmt = _controllableElements.erase(elmt);
        hasDeleted = true;
      }
    }

    if (!hasDeleted)
      elmt++;
  }

  for (auto elmt = _deadControllableElements.begin(); elmt != _deadControllableElements.end(); ) {
    bool hasDeleted = false;
    for (auto toDel = elementsToDelete.begin(); toDel != elementsToDelete.end(); toDel++) {
      if (*toDel == *elmt) {
        elmt = _deadControllableElements.erase(elmt);
        hasDeleted = true;
      }
    }

    if (!hasDeleted)
      elmt++;
  }
}

glm::vec2 Engine::get2DCoord(glm::ivec2 screenTarget) {
  Camera& cam = Camera::getInstance();

  screenTarget = glm::ivec2(screenTarget.x * cam.getW() / cam.getWindowW(),
                            screenTarget.y * cam.getH() / cam.getWindowH());

  screenTarget.y = cam.getH() - screenTarget.y; // Inverted coordinates

  _depthInColorBufferFBO.bind();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  _depthInColorBufferShader.bind();
  _depthTexturedRectangle->draw();
  Shader::unbind();

  GLfloat depth;
  glReadPixels(screenTarget.x, screenTarget.y, 1, 1, GL_RED, GL_FLOAT, &depth);

  FrameBufferObject::unbind();

  glm::vec3 modelCoord = glm::unProject(glm::vec3(screenTarget.x, screenTarget.y,depth),
    glm::mat4(1.f), cam.getViewProjectionMatrix(),
    glm::vec4(0,0, cam.getW(), cam.getH()));

  return glm::vec2( modelCoord.x, modelCoord.y);
}

glm::vec3 Engine::getNormalOnCameraPointedPos() const {
  Camera& cam = Camera::getInstance();
  glm::uvec2 chunkPos = ut::convertToChunkCoords(cam.getPointedPos());

  return _terrain[chunkPos.x*NB_CHUNKS + chunkPos.y]->getNorm(cam.getPointedPos());
}

float Engine::getHeight(glm::vec2 pos) const {
  if (pos.x < 0 || pos.y < 0 || pos.x > MAX_COORD || pos.y > MAX_COORD)
    return 0.f;

  glm::uvec2 chunkPos = ut::convertToChunkCoords(pos);

  return _terrain[chunkPos.x * NB_CHUNKS + chunkPos.y]->getHeight(pos);
}
