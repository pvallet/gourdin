#include "game.h"

#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"

#include <iostream>
#include <cstdlib>
#include <ctime>

#define CHUNK_BEGIN_X 12
#define CHUNK_BEGIN_Y 23

#define MIN_ANTILOPE_PROX 5.f
#define HERD_RADIUS 10.f // for 10 antilopes

Game::Game() :
  _hmapShader("src/shaders/heightmap.vert", "src/shaders/heightmap.frag"),
  _igEShader ("src/shaders/igElement.vert", "src/shaders/igElement.frag") {}

void Game::init() {
  srand(time(NULL));

  _hmapShader.load();
  _igEShader.load();

  _terrainTexManager.loadFolder(NB_BIOMES, "res/terrain/");
  _treeTexManager.    load("res/trees/");
  _antilopeTexManager.load("res/animals/antilope/");
  _lionTexManager.    load("res/animals/lion/");
  _map.load("res/map/");

  std::vector<ChunkStatus> initializer(NB_CHUNKS, NOT_GENERATED);
  _chunkStatus.resize(NB_CHUNKS, initializer);

  for (size_t i = 0; i < NB_CHUNKS; i++) {
    std::vector<std::unique_ptr<Chunk> > initializer2(NB_CHUNKS);
    _terrain.push_back(std::move(initializer2));
  }

  Camera& cam = Camera::getInstance();
  cam.setPointedPos(sf::Vector2f(CHUNK_BEGIN_X * CHUNK_SIZE + CHUNK_SIZE / 2,
                                 CHUNK_BEGIN_Y * CHUNK_SIZE + CHUNK_SIZE / 2));

  generateHerd(     sf::Vector2f(CHUNK_BEGIN_X * CHUNK_SIZE + CHUNK_SIZE / 2,
                                 CHUNK_BEGIN_Y * CHUNK_SIZE + CHUNK_SIZE / 2), 20);
}

void Game::generateHeightmap(size_t x, size_t y) {
  Heightmap* newHeightmap = new Heightmap(x, y, _terrainTexManager, _map);
  newHeightmap->generate();
  _terrain[x][y] = std::unique_ptr<Chunk>(newHeightmap);
  _chunkStatus[x][y] = EDGE;
  generateForests(x,y);
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

      if (_chunkStatus[tmp.x][tmp.y] == NOT_GENERATED) {
        // If the chunk to be generated is not handled by the map, we don't do anything
        if (tmp.x < 0 || tmp.x >= NB_CHUNKS || tmp.y < 0 || tmp.y >= NB_CHUNKS);


        else if (_map.getClosestCenter(sf::Vector2f(tmp.x * CHUNK_SIZE, tmp.y * CHUNK_SIZE))->biome == OCEAN &&
                 _map.getClosestCenter(sf::Vector2f(tmp.x * CHUNK_SIZE, (tmp.y+1) * CHUNK_SIZE))->biome == OCEAN &&
                 _map.getClosestCenter(sf::Vector2f((tmp.x+1) * CHUNK_SIZE, tmp.y * CHUNK_SIZE))->biome == OCEAN &&
                 _map.getClosestCenter(sf::Vector2f((tmp.x+1) * CHUNK_SIZE, (tmp.y+1) * CHUNK_SIZE))->biome == OCEAN) {

          _terrain[tmp.x][tmp.y] = std::unique_ptr<Chunk>(new Ocean(tmp.x, tmp.y, _terrainTexManager.getTexID(OCEAN)));
          _chunkStatus[tmp.x][tmp.y] = EDGE;
        }

        else
          generateHeightmap(tmp.x,tmp.y);
      }
    }
  }

  _chunkStatus[x][y] = NOT_VISIBLE;
}

void Game::update(sf::Time elapsed) {
  Camera& cam = Camera::getInstance();
  int camPosX = cam.getPointedPos().x < 0 ? cam.getPointedPos().x / CHUNK_SIZE - 1 : cam.getPointedPos().x / CHUNK_SIZE;
  int camPosY = cam.getPointedPos().y < 0 ? cam.getPointedPos().y / CHUNK_SIZE - 1 : cam.getPointedPos().y / CHUNK_SIZE;

  // Update camera
  if (_chunkStatus[camPosX][camPosY] == NOT_GENERATED)
    generateHeightmap(camPosX, camPosY);

  cam.setHeight( _terrain[camPosX][camPosY]
    ->getHeight(cam.getPointedPos().x - CHUNK_SIZE * camPosX,
                cam.getPointedPos().y - CHUNK_SIZE * camPosY));

  // Update terrains
  #pragma omp parallel for collapse(2)
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

  // Compute moving elements interactions
  for (size_t i = 0; i < _igMovingElements.size(); i++) {
    Antilope* atlp;
    Lion* lion;

    if (atlp = dynamic_cast<Antilope*>(_igMovingElements[i]))
      atlp->updateState(_igMovingElements);

    else if (lion = dynamic_cast<Lion*>(_igMovingElements[i]))
      lion->kill(_igMovingElements);
  }

  _visibleElmts.clear();

  // Update in game elements characteristics
  for (unsigned int i = 0 ; i < _igElements.size() ; i++) {
    _igElements[i]->update(elapsed, cam.getTheta()); // Choose the right sprite and update pos

    int chunkPosX = _igElements[i]->getPos().x / CHUNK_SIZE;
    int chunkPosY = _igElements[i]->getPos().y / CHUNK_SIZE;

    if (_chunkStatus[chunkPosX][chunkPosY] == VISIBLE) {

      // No test yet to see if the element can move to its new pos (no collision)
      float newHeight =   _terrain[chunkPosX][chunkPosY]
                           ->getHeight(_igElements[i]->getPos().x - (int) CHUNK_SIZE * chunkPosX,
                                       _igElements[i]->getPos().y - (int) CHUNK_SIZE * chunkPosY);

      // Calculate new corners
      glm::vec3 corners3[4];
      float width = _igElements[i]->getSize().x;

      corners3[0] = glm::vec3(  _igElements[i]->getPos().x - sin(cam.getTheta()*M_PI/180.)*width/2,
                                _igElements[i]->getPos().y + cos(cam.getTheta()*M_PI/180.)*width/2,
                                newHeight + _igElements[i]->getSize().y);

      corners3[1] = glm::vec3(  _igElements[i]->getPos().x + sin(cam.getTheta()*M_PI/180.)*width/2,
                                _igElements[i]->getPos().y - cos(cam.getTheta()*M_PI/180.)*width/2,
                                newHeight + _igElements[i]->getSize().y);

      corners3[2] = glm::vec3(  _igElements[i]->getPos().x + sin(cam.getTheta()*M_PI/180.)*width/2,
                                _igElements[i]->getPos().y - cos(cam.getTheta()*M_PI/180.)*width/2,
                                newHeight);

      corners3[3] = glm::vec3(  _igElements[i]->getPos().x - sin(cam.getTheta()*M_PI/180.)*width/2,
                                _igElements[i]->getPos().y + cos(cam.getTheta()*M_PI/180.)*width/2,
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

  // Heightmap

  glUseProgram(_hmapShader.getProgramID());

  for (size_t i = 0; i < NB_CHUNKS; i++) {
    for (size_t j = 0; j < NB_CHUNKS; j++) {
      if (_chunkStatus[i][j] == VISIBLE) {
        glm::mat4 modelview = glm::translate(glm::mat4(1.f),
          glm::vec3(CHUNK_SIZE * i, CHUNK_SIZE * j, 0.f)
        );

        _hmapShader.sendModelMatrix(modelview);

        _terrain[i][j]->draw();

      }
    }
  }

  // igElements

  glUseProgram(_igEShader.getProgramID());
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  for (auto it = _visibleElmts.begin() ; it != _visibleElmts.end() ; ++it) {
    _hmapShader.sendModelMatrix(glm::mat4(1.f));
    (*it)->draw();
  }

  glDisable(GL_BLEND);

  glUseProgram(0);

  _glCheckError();
}

void Game::select(sf::IntRect rect, bool add) {
  if (!add)
    _selectedElmts.clear();

  for (unsigned int i = 0 ; i < _igElements.size() ; i++) {
    if (_selectedElmts.find(_igElements[i].get()) == _selectedElmts.end()) { // _igElements[i] is not selected yet, we can bother to calculate
      sf::IntRect c = _igElements[i]->get2DCorners();

      int centerX, centerY;

      centerX = c.left + c.width / 2;
      centerY = c.top + c.height / 2;

      if (rect.contains(centerX, centerY)) {
        if (dynamic_cast<Lion*>(_igElements[i].get()))
          _selectedElmts.insert(_igElements[i].get());
      }

      else if (   c.contains(rect.left, rect.top) ||
                  c.contains(rect.left + rect.width, rect.top) ||
                  c.contains(rect.left + rect.width, rect.top + rect.height) ||
                  c.contains(rect.left, rect.top + rect.height)  ) {
        if (dynamic_cast<Lion*>(_igElements[i].get()))
          _selectedElmts.insert(_igElements[i].get());
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
  generateHeightmap(newAimedPos.x / CHUNK_SIZE, newAimedPos.y / CHUNK_SIZE);
  Camera& cam = Camera::getInstance();
  cam.setPointedPos(newAimedPos);
}

void Game::addLion(sf::Vector2i screenTarget) {
  _igMovingElements.push_back(new Lion(get2DCoord(screenTarget), AnimationManager(_lionTexManager)));
  _igElements.push_back(std::unique_ptr<igElement>(_igMovingElements.back()));
}

void Game::generateHerd(sf::Vector2f pos, size_t count) {
  float r, theta;
  sf::Vector2f p, diff;
  bool add;

  std::vector<Antilope*> tmp;

  for (size_t i = 0 ; i < count ; i++) {
    add = true;
    r = sqrt(RANDOMF) * HERD_RADIUS * sqrt(count);
    theta = RANDOMF * 2*M_PI;

    p.x = pos.x + r*cos(theta);
    p.y = pos.y + r*sin(theta);

    for (unsigned int j = 0 ; j < tmp.size() ; j++) {
      diff = tmp[j]->getPos() - p;

      if (diff.x * diff.x + diff.y * diff.y < MIN_ANTILOPE_PROX) {
        i--;
        add = false;
      }
    }

    if (add) {
      tmp.push_back(new Antilope(p, AnimationManager(_antilopeTexManager)));
    }
  }

  for (size_t i = 0 ; i < count ; i++) {
    _igElements.push_back(std::unique_ptr<igElement>(tmp[i]));
    _igMovingElements.push_back(tmp[i]);
  }
}

void Game::generateForests(size_t x, size_t y) {
  float r, theta;
  sf::Vector2f p, diff;
  bool add;
  size_t count, nbTrees;

  std::vector<Tree*> tmp;

  std::vector<Center*> centers = _map.getCentersInChunk(x,y);

  for (unsigned int i = 0 ; i < centers.size() ; i++) {
    if (centers[i]->biome >= 11) { // No forests in other biomes
      count = 0;
      tmp.clear();
      nbTrees = _treeTexManager.getExtension(centers[i]->biome);
      nbTrees *= 1.5;

      for (size_t j = 0 ; j < nbTrees ; j++) {
        add = true;
        r = sqrt(RANDOMF) * _treeTexManager.getExtension(centers[i]->biome) * sqrt(nbTrees);
        theta = RANDOMF * 2*M_PI;

        p.x = centers[i]->x + r*cos(theta);
        p.y = centers[i]->y + r*sin(theta);

        for (unsigned int k = 0 ; k < tmp.size() ; k++) {
          diff = tmp[k]->getPos() - p;

          if (diff.x * diff.x + diff.y * diff.y < _treeTexManager.getDensity(centers[i]->biome) ||
            _map.getClosestCenter(p)->biome != centers[i]->biome) {
            add = false;
          }
        }

        if (add) {
          count++;
          tmp.push_back(new Tree(p, &_treeTexManager, centers[i]->biome,
            (int) ((RANDOMF - 0.01f) * _treeTexManager.getNBTrees(centers[i]->biome))));
        }
      }

      for (size_t j = 0 ; j < count ; j++) {
        _igElements.push_back(std::unique_ptr<igElement>(tmp[j]));
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
    glm::mat4(1.), cam.getViewProjectionMatrix(),
    glm::vec4(0,0, cam.getW(), cam.getH()));

  return sf::Vector2f( modelCoord.x, modelCoord.y);
}
