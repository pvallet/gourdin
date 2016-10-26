#include "game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define MIN_ANTILOPE_PROX 5.f
#define HERD_RADIUS 10.f // for 10 antilopes
#define CHUNK_BEGIN_X 11
#define CHUNK_BEGIN_Y 23

Game::Game(Camera* camera, Map* map) :
  _cam(camera),
  _map(map),
  _hmapShader("src/shaders/heightmap.vert", "src/shaders/heightmap.frag"),
  _igEShader ("src/shaders/igElement.vert", "src/shaders/igElement.frag") {}

void Game::init() {
  _hmapShader.load();
  _igEShader.load();

  _terrainTexManager.loadFolder(NB_BIOMES, "res/terrain/");
  _treeTexManager.load("res/trees/");

  _cam->setPointedPos(sf::Vector2f(CHUNK_BEGIN_X*CHUNK_SIZE,CHUNK_BEGIN_Y*CHUNK_SIZE));

  Heightmap* hmap[4];

  srand(time(NULL));

  hmap[0] = new Heightmap(sf::Vector2i(CHUNK_BEGIN_X,CHUNK_BEGIN_Y), rand(), &_terrainTexManager, _map);
  hmap[1] = new Heightmap(sf::Vector2i(CHUNK_BEGIN_X,CHUNK_BEGIN_Y - 1), rand(), &_terrainTexManager, _map);
  hmap[2] = new Heightmap(sf::Vector2i(CHUNK_BEGIN_X - 1,CHUNK_BEGIN_Y), rand(), &_terrainTexManager, _map);
  hmap[3] = new Heightmap(sf::Vector2i(CHUNK_BEGIN_X - 1,CHUNK_BEGIN_Y - 1), rand(), &_terrainTexManager, _map);

  hmap[0]->generate(std::vector<Constraint>());
  _terrain.insert(std::pair<sf::Vector2i, Chunk*>(hmap[0]->getChunkPos(), hmap[0]));
  _terrainBorder.insert(hmap[0]->getChunkPos());

  for (size_t i = 1 ; i < 4 ; i++) {
    std::vector<Constraint> c;

    for (size_t j = 0 ; j < i ; j++)
      c.push_back(hmap[j]->getConstraint(hmap[i]->getChunkPos()));

    hmap[i]->generate(c);
    _terrain.insert(std::pair<sf::Vector2i, Chunk*>(hmap[i]->getChunkPos(), hmap[i]));
    _terrainBorder.insert(hmap[i]->getChunkPos());
    generateForests(hmap[i]->getChunkPos());
  }

  _antilopeTexManager.load("res/animals/antilope/");
  _lionTexManager.load("res/animals/lion/");

  generateHerd(sf::Vector2<double>(CHUNK_BEGIN_X * CHUNK_SIZE, CHUNK_BEGIN_Y * CHUNK_SIZE), 20);
}

void Game::generateHeightmap(sf::Vector2i pos) {
  sf::Vector2i tmp;
  std::vector<Constraint> c;

  for (size_t j = 0 ; j < 4 ; j++) { // Get constraints
    tmp = neighbour(pos,j);
    std::map<sf::Vector2i, Chunk*>::iterator it = _terrain.find(tmp);

    if (it != _terrain.end()) {
      c.push_back(it->second->getConstraint(pos));
    }
  }

  Heightmap* hmap = new Heightmap(pos, rand(), &_terrainTexManager, _map);
  hmap->generate(c);
  _terrain.insert(std::pair<sf::Vector2i, Chunk*>(hmap->getChunkPos(), hmap));
  _terrainBorder.insert(hmap->getChunkPos());
}

void Game::generateNeighbourChunks(sf::Vector2i pos) {
  if (_terrainBorder.find(pos) != _terrainBorder.end()) {
    sf::Vector2i tmp;
    for (size_t i = 0; i < 4; i++) {
      tmp = neighbour(pos,i);

      if (_terrain.find(tmp) == _terrain.end()) { // We only add the neighbour if it's not already generated
        // If the chunk to be generated is not handled by the _map, we generate an ocean
        if (tmp.x < 0 || tmp.x >= _map->getNbChunks() || tmp.y < 0 || tmp.y >= _map->getNbChunks()) {
          _terrain.insert(std::pair<sf::Vector2i, Chunk*>(tmp, new Ocean(tmp, _terrainTexManager.getTexID(OCEAN))));
          _terrainBorder.insert(tmp);
        }

        else if (_map->getClosestCenter(sf::Vector2<double>(tmp.x * CHUNK_SIZE, tmp.y * CHUNK_SIZE))->biome == OCEAN &&
                 _map->getClosestCenter(sf::Vector2<double>(tmp.x * CHUNK_SIZE, (tmp.y+1) * CHUNK_SIZE))->biome == OCEAN &&
                 _map->getClosestCenter(sf::Vector2<double>((tmp.x+1) * CHUNK_SIZE, tmp.y * CHUNK_SIZE))->biome == OCEAN &&
                 _map->getClosestCenter(sf::Vector2<double>((tmp.x+1) * CHUNK_SIZE, (tmp.y+1) * CHUNK_SIZE))->biome == OCEAN) {

          _terrain.insert(std::pair<sf::Vector2i, Chunk*>(tmp, new Ocean(tmp, _terrainTexManager.getTexID(OCEAN))));
          _terrainBorder.insert(tmp);
        }

        else {
          generateHeightmap(tmp);
          generateForests(tmp);
        }
      }
    }

    _terrainBorder.erase(pos);
  }
}

sf::Vector2i Game::neighbour(sf::Vector2i pos, int index) const {
  switch(index) {
    case 0:
      return sf::Vector2i(pos.x-1,pos.y);
      break;
    case 1:
      return sf::Vector2i(pos.x+1,pos.y);
      break;
    case 2:
      return sf::Vector2i(pos.x,pos.y-1);
      break;
    case 3:
      return sf::Vector2i(pos.x,pos.y+1);
      break;
    default:
      return sf::Vector2i(pos.x,pos.y);
      std::cerr << "Error in Game::neighbour: Index out of bounds" << std::endl;
      break;
  }
}

void Game::update(sf::Time elapsed) {
  int camPosX = _cam->getPointedPos().x < 0 ? _cam->getPointedPos().x / CHUNK_SIZE - 1 : _cam->getPointedPos().x / CHUNK_SIZE;
  int camPosY = _cam->getPointedPos().y < 0 ? _cam->getPointedPos().y / CHUNK_SIZE - 1 : _cam->getPointedPos().y / CHUNK_SIZE;

  _cam->setHeight( _terrain.at(sf::Vector2i(camPosX, camPosY))
                          ->getHeight(_cam->getPointedPos().x - CHUNK_SIZE * camPosX,
                                      _cam->getPointedPos().y - CHUNK_SIZE * camPosY));

  for (auto it = _terrainBorder.begin() ; it != _terrainBorder.end() ; ++it) {
    if (_terrain.at(*it)->isVisible())
      generateNeighbourChunks(*it);
  }

  for (unsigned int i = 0 ; i < _e.size() ; i++) {
    if (_e[i]->getAbstractType() != igE) {
      igMovingElement* igM = (igMovingElement*) _e[i];
      if (igM->getMovingType() == PREY) {
        Antilope* atlp = (Antilope*) igM;
        atlp->updateState(_e);
      }

      else if (igM->getMovingType() == HUNTER) {
        Lion* lion = (Lion*) igM;
        lion->kill(_e);
      }
    }
  }

  for (auto it = _terrain.begin() ; it != _terrain.end() ; ++it) {
    it->second->computeCulling(_cam);
  }

  _vis.clear();

  for (unsigned int i = 0 ; i < _e.size() ; i++) {
    int chunkPosX = _e[i]->getPos().x / CHUNK_SIZE;
    int chunkPosY = _e[i]->getPos().y / CHUNK_SIZE;

    if (_terrain.find(sf::Vector2i(chunkPosX, chunkPosY)) != _terrain.end() &&
        _terrain.at(sf::Vector2i(chunkPosX, chunkPosY))->isVisible()) {

      _e[i]->update(elapsed, _cam->getTheta()); // Choose the right sprite and update pos

      chunkPosX = _e[i]->getPos().x / CHUNK_SIZE;
      chunkPosY = _e[i]->getPos().y / CHUNK_SIZE;

      // No test yet to see if the element can move to its new pos (no collision)
      double newHeight =   _terrain.at(sf::Vector2i(chunkPosX, chunkPosY))
                           ->getHeight(_e[i]->getPos().x - (int) CHUNK_SIZE * chunkPosX,
                                       _e[i]->getPos().y - (int) CHUNK_SIZE * chunkPosY);

      // Calculate new corners
      glm::vec3 corners3[4];

      float width = _e[i]->getSize().x;


      corners3[0] = glm::vec3(  _e[i]->getPos().x - sin(_cam->getTheta()*M_PI/180.)*width/2,
                                _e[i]->getPos().y + cos(_cam->getTheta()*M_PI/180.)*width/2,
                                newHeight + _e[i]->getSize().y);

      corners3[1] = glm::vec3(  _e[i]->getPos().x + sin(_cam->getTheta()*M_PI/180.)*width/2,
                                _e[i]->getPos().y - cos(_cam->getTheta()*M_PI/180.)*width/2,
                                newHeight + _e[i]->getSize().y);

      corners3[2] = glm::vec3(  _e[i]->getPos().x + sin(_cam->getTheta()*M_PI/180.)*width/2,
                                _e[i]->getPos().y - cos(_cam->getTheta()*M_PI/180.)*width/2,
                                newHeight);

      corners3[3] = glm::vec3(  _e[i]->getPos().x - sin(_cam->getTheta()*M_PI/180.)*width/2,
                                _e[i]->getPos().y + cos(_cam->getTheta()*M_PI/180.)*width/2,
                                newHeight);


      _e[i]->set3DCorners(corners3);

      // Calculate their projections

      glm::mat4 viewProj = _cam->getViewProjectionMatrix();

      glm::vec3 cornersProjNorm[4];
      for (size_t i = 0; i < 4; i++) {
        cornersProjNorm[i] = glm::project(corners3[i], glm::mat4(1.f), viewProj,
          glm::vec4(0,0,_cam->getW(),_cam->getH()));
      }

      double left, top, right, bot, depth;

      left  = cornersProjNorm[0].x;
      top   = cornersProjNorm[0].y;
      right = cornersProjNorm[1].x;
      bot   = cornersProjNorm[3].y;
      depth = cornersProjNorm[3].z;

      top = _cam->getH()-top;
      bot = _cam->getH()-bot;

      sf::IntRect cornersRect((int) left, (int) top, (int) right-left, (int) bot-top);

      _e[i]->set2DCorners(cornersRect);
      _e[i]->setDepth(depth);

      _vis.insert(_e[i]);
    }
  }
}

void Game::render() const {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Heightmap

  glUseProgram(_hmapShader.getProgramID());

  for(auto it = _terrain.begin() ; it != _terrain.end() ; ++it) {
    if (it->second->isVisible()) {
      glm::mat4 modelview = glm::translate(glm::mat4(1.f),
        glm::vec3(CHUNK_SIZE * it->first.x, CHUNK_SIZE * it->first.y, 0.f)
      );

      _hmapShader.sendModelMatrix(_cam, modelview);

      // _hmapShader.sendModelMatrix(_cam, glm::mat4(1.f));


      it->second->draw();

    }
  }

  // igElements

  glUseProgram(_igEShader.getProgramID());
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  for (auto it = _vis.begin() ; it != _vis.end() ; ++it) {
    _hmapShader.sendModelMatrix(_cam, glm::mat4(1.f));
    (*it)->draw();
  }

  glDisable(GL_BLEND);

  glUseProgram(0);
}

void Game::select(sf::IntRect rect, bool add) {
  if (!add)
    _sel.clear();

  for (unsigned int i = 0 ; i < _e.size() ; i++) {
    if (_sel.find(_e[i]) == _sel.end()) { // _e[i] is not selected yet, we can bother to calculate
      sf::IntRect c = _e[i]->get2DCorners();

      int centerX, centerY;

      centerX = c.left + c.width / 2;
      centerY = c.top + c.height / 2;

      if (rect.contains(centerX, centerY)) {
        if (_e[i]->getAbstractType() == CTRL)
          _sel.insert(_e[i]);
      }

      else if (   c.contains(rect.left, rect.top) ||
                  c.contains(rect.left + rect.width, rect.top) ||
                  c.contains(rect.left + rect.width, rect.top + rect.height) ||
                  c.contains(rect.left, rect.top + rect.height)  ) {
        if (_e[i]->getAbstractType() == CTRL)
          _sel.insert(_e[i]);
      }
    }
  }
}

void Game::moveSelection(sf::Vector2i screenTarget) {
  sf::Vector2<double> target = get2DCoord(screenTarget);

  for(auto it = _sel.begin(); it != _sel.end(); ++it) {
    if ((*it)->getAbstractType() == CTRL) {
      Controllable* tmp = (Controllable*) *it;
      tmp->setTarget(target);
    }
  }
}

void Game::moveCamera(sf::Vector2f newAimedPos) {
  generateHeightmap(sf::Vector2i(newAimedPos.x / CHUNK_SIZE, newAimedPos.y / CHUNK_SIZE));
  _cam->setPointedPos(newAimedPos);
}

void Game::addLion(sf::Vector2i screenTarget) {
  _e.push_back(new Lion(get2DCoord(screenTarget), AnimationManager(_lionTexManager)));
}

void Game::generateHerd(sf::Vector2<double> pos, size_t count) {
  srand(time(NULL));
  double r, theta;
  sf::Vector2<double> p, diff;
  bool add;

  std::vector<Antilope*> tmp;

  for (size_t i = 0 ; i < count ; i++) {
    add = true;
    r = sqrt(randomD()) * HERD_RADIUS * sqrt(count);
    theta = randomD() * 2*M_PI;

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
    _e.push_back(tmp[i]);
  }
}

void Game::generateForests(sf::Vector2i pos) {
  srand(time(NULL));
  double r, theta;
  sf::Vector2<double> p, diff;
  bool add;
  size_t count, nbTrees;

  std::vector<Tree*> tmp;

  std::vector<Center*> centers = _map->getCentersInChunk(pos);

  for (unsigned int i = 0 ; i < centers.size() ; i++) {
    if (centers[i]->biome >= 11) { // No forests in other biomes
      count = 0;
      tmp.clear();
      nbTrees = _treeTexManager.getExtension(centers[i]->biome);
      nbTrees *= 1.5;

      for (size_t j = 0 ; j < nbTrees ; j++) {
        add = true;
        r = sqrt(randomD()) * _treeTexManager.getExtension(centers[i]->biome) * sqrt(nbTrees);
        theta = randomD() * 2*M_PI;

        p.x = centers[i]->x + r*cos(theta);
        p.y = centers[i]->y + r*sin(theta);

        for (unsigned int k = 0 ; k < tmp.size() ; k++) {
          diff = tmp[k]->getPos() - p;

          if (diff.x * diff.x + diff.y * diff.y < _treeTexManager.getDensity(centers[i]->biome) ||
            _map->getClosestCenter(p)->biome != centers[i]->biome) {
            add = false;
          }
        }

        if (add) {
          count++;
          tmp.push_back(new Tree(p, &_treeTexManager, centers[i]->biome,
            (int) ((randomD() - 0.01f) * _treeTexManager.getNBTrees(centers[i]->biome))));
        }
      }

      for (size_t j = 0 ; j < count ; j++) {
        _e.push_back(tmp[j]);
      }
    }
  }
}

sf::Vector2<double> Game::get2DCoord(sf::Vector2i screenTarget) const {
  screenTarget.y = _cam->getH() - screenTarget.y; // Inverted coordinates

  GLfloat d;
  glReadPixels(screenTarget.x, screenTarget.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &d);

  glm::vec3 modelCoord = glm::unProject(glm::vec3(screenTarget.x, screenTarget.y,d),
    glm::mat4(1.), _cam->getViewProjectionMatrix(),
    glm::vec4(0,0,_cam->getW(),_cam->getH()));

  return sf::Vector2<double>( modelCoord.x, modelCoord.y);
}
