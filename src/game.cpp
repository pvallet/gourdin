#include "game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define MIN_ANTILOPE_PROX 5.f
#define HERD_RADIUS 10.f // for 10 antilopes
#define CHUNK_BEGIN_X 30
#define CHUNK_BEGIN_Y 11

Game::Game(Camera* camera, Map* map) :
  _cam(camera),
  _map(map),
  _hmapShader("src/shaders/testShader.vert", "src/shaders/testShader.frag") {}

void Game::init() {
  _hmapShader.load();
  _terrainTexManager.load("res/terrain/"),
  _treeTexManager.load("res/trees/"),

  _cam->translate(CHUNK_BEGIN_Y*CHUNK_SIZE,CHUNK_BEGIN_X*CHUNK_SIZE);

  Heightmap* hmap[4];

  srand(time(NULL));

  hmap[0] = new Heightmap(sf::Vector2i(CHUNK_BEGIN_X,CHUNK_BEGIN_Y), rand(), &_terrainTexManager, _map);
  hmap[1] = new Heightmap(sf::Vector2i(CHUNK_BEGIN_X,CHUNK_BEGIN_Y - 1), rand(), &_terrainTexManager, _map);
  hmap[2] = new Heightmap(sf::Vector2i(CHUNK_BEGIN_X - 1,CHUNK_BEGIN_Y), rand(), &_terrainTexManager, _map);
  hmap[3] = new Heightmap(sf::Vector2i(CHUNK_BEGIN_X - 1,CHUNK_BEGIN_Y - 1), rand(), &_terrainTexManager, _map);

  hmap[0]->generate(std::vector<Constraint>());
  _terrain.insert(std::pair<sf::Vector2i, Chunk*>(hmap[0]->getChunkPos(), hmap[0]));
  _terrainBorder.insert(hmap[0]->getChunkPos());

  for (int i = 1 ; i < 4 ; i++) {
    std::vector<Constraint> c;

    for (int j = 0 ; j < i ; j++)
      c.push_back(hmap[j]->getConstraint(hmap[i]->getChunkPos()));

    hmap[i]->generate(c);
    _terrain.insert(std::pair<sf::Vector2i, Chunk*>(hmap[i]->getChunkPos(), hmap[i]));
    _terrainBorder.insert(hmap[i]->getChunkPos());
    generateForests(hmap[i]->getChunkPos());
  }

  _skybox = new Skybox("res/skybox/skybox", _cam);

  sf::Color mask(0, 151, 135);

  std::vector<std::string> lionSheets;
  lionSheets.push_back("res/animals/lion/wait.png");
  lionSheets.push_back("res/animals/lion/walk.png");
  lionSheets.push_back("res/animals/lion/die.png");
  lionSheets.push_back("res/animals/lion/run.png");
  lionSheets.push_back("res/animals/lion/attack.png");

  for (unsigned int i = 0 ; i < lionSheets.size() ; i++) {
    sf::Image img;

    if (!img.loadFromFile(lionSheets[i])) {
      std::cerr << "Unable to open file" << std::endl;
    }

    sf::Texture* curTex = new sf::Texture();

    img.createMaskFromColor(mask);

    curTex->loadFromImage(img);
    curTex->setSmooth(true);

    _lionTex.push_back(curTex);
  }

  std::vector<std::string> antilopeSheets;
  antilopeSheets.push_back("res/animals/antilope/wait.png");
  antilopeSheets.push_back("res/animals/antilope/walk.png");
  antilopeSheets.push_back("res/animals/antilope/die.png");
  antilopeSheets.push_back("res/animals/antilope/run.png");

  for (unsigned int i = 0 ; i < antilopeSheets.size() ; i++) {
    sf::Image img;

    if (!img.loadFromFile(antilopeSheets[i])) {
      std::cerr << "Unable to open file" << std::endl;
    }

    sf::Texture* curTex = new sf::Texture();

    img.createMaskFromColor(mask);

    curTex->loadFromImage(img);
    curTex->setSmooth(true);

    _antilopeTex.push_back(curTex);
  }

  generateHerd(sf::Vector2f(CHUNK_BEGIN_X * CHUNK_SIZE, CHUNK_BEGIN_Y * CHUNK_SIZE), 20);
}

Game::~Game() {
	for(auto it = _terrain.begin() ; it != _terrain.end() ; ++it) {
      //// delete it->second;
  }

  // delete _skybox;
  for (unsigned int i = 0 ; i < _e.size() ; i++) {
    // delete _e[i];
  }
}

void Game::generateHeightmap(sf::Vector2i pos) {
  sf::Vector2i tmp;
  std::vector<Constraint> c;

  for (int j = 0 ; j < 4 ; j++) { // Get constraints
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
    for (int i = 0; i < 4; i++) {
      tmp = neighbour(pos,i);

      if (_terrain.find(tmp) == _terrain.end()) { // We only add the neighbour if it's not already generated
        // If the chunk to be generated is not handled by the _map, we generate an ocean
        if (tmp.x < 0 || tmp.x >= _map->getNbChunks() || tmp.y < 0 || tmp.y >= _map->getNbChunks()) {
          _terrain.insert(std::pair<sf::Vector2i, Chunk*>(tmp, new Ocean(tmp, _terrainTexManager.getTexIndex(OCEAN))));
          _terrainBorder.insert(tmp);
        }

        else if (_map->getClosestCenter(sf::Vector2<double>(tmp.x * CHUNK_SIZE, tmp.y * CHUNK_SIZE))->biome == OCEAN &&
                 _map->getClosestCenter(sf::Vector2<double>(tmp.x * CHUNK_SIZE, (tmp.y+1) * CHUNK_SIZE))->biome == OCEAN &&
                 _map->getClosestCenter(sf::Vector2<double>((tmp.x+1) * CHUNK_SIZE, tmp.y * CHUNK_SIZE))->biome == OCEAN &&
                 _map->getClosestCenter(sf::Vector2<double>((tmp.x+1) * CHUNK_SIZE, (tmp.y+1) * CHUNK_SIZE))->biome == OCEAN) {

          _terrain.insert(std::pair<sf::Vector2i, Chunk*>(tmp, new Ocean(tmp, _terrainTexManager.getTexIndex(OCEAN))));
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
  _cam->setHeight( _terrain[sf::Vector2i(camPosX, camPosY)]
                          ->getHeight(_cam->getPointedPos().x - CHUNK_SIZE * camPosX,
                                      _cam->getPointedPos().y - CHUNK_SIZE * camPosY));

  for (auto it = _terrainBorder.begin() ; it != _terrainBorder.end() ; ++it) {
      if (_terrain[(*it)]->isVisible())
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
      it->second->calculateFrustum(_cam);
  }

  _vis.clear();

  for (unsigned int i = 0 ; i < _e.size() ; i++) {
    int chunkPosX = _e[i]->getPos().x / CHUNK_SIZE;
    int chunkPosY = _e[i]->getPos().y / CHUNK_SIZE;

    if (_terrain.find(sf::Vector2i(chunkPosX, chunkPosY)) != _terrain.end() &&
        _terrain.at(sf::Vector2i(chunkPosX, chunkPosY))->isVisible()) {

      //std::cout << chunkPosX << " " << chunkPosY << std::endl;

      _e[i]->update(elapsed, _cam->getTheta()); // Choose the right sprite and update pos

      chunkPosX = _e[i]->getPos().x / CHUNK_SIZE;
      chunkPosY = _e[i]->getPos().y / CHUNK_SIZE;

      // No test yet to see if the element can move to its new pos (no collision)
      double newHeight =   _terrain[sf::Vector2i(chunkPosX, chunkPosY)]
                           ->getHeight(_e[i]->getPos().x - (int) CHUNK_SIZE * chunkPosX,
                                       _e[i]->getPos().y - (int) CHUNK_SIZE * chunkPosY);

      // Calculate new corners
      glm::vec3 corners3[4];

      float width = _e[i]->getW();

      corners3[0] = glm::vec3(  _e[i]->getPos().x + sin(_cam->getTheta()*M_PI/180.)*width/2,
                                newHeight + _e[i]->getH(),
                                _e[i]->getPos().y - cos(_cam->getTheta()*M_PI/180.)*width/2);


      corners3[1] = glm::vec3(  _e[i]->getPos().x - sin(_cam->getTheta()*M_PI/180.)*width/2,
                                newHeight + _e[i]->getH(),
                                _e[i]->getPos().y + cos(_cam->getTheta()*M_PI/180.)*width/2);


      corners3[2] = glm::vec3(  _e[i]->getPos().x - sin(_cam->getTheta()*M_PI/180.)*width/2,
                                newHeight,
                                _e[i]->getPos().y + cos(_cam->getTheta()*M_PI/180.)*width/2);


      corners3[3] = glm::vec3(  _e[i]->getPos().x + sin(_cam->getTheta()*M_PI/180.)*width/2,
                                newHeight,
                                _e[i]->getPos().y - cos(_cam->getTheta()*M_PI/180.)*width/2);

      _e[i]->set3DCorners(corners3);

      // Calculate their projections


      // gluProject(corners3[0].x,corners3[0].y,corners3[0].z,modelview,projection,viewport,&left,  &top,   &trash);
      // gluProject(corners3[1].x,corners3[1].y,corners3[1].z,modelview,projection,viewport,&right, &trash, &trash);
      // gluProject(corners3[3].x,corners3[3].y,corners3[3].z,modelview,projection,viewport,&trash, &bot,   &depth);

      // TODO optimize with only 2 matrix multiplications

      glm::mat4 viewProjection = _cam->getViewProjectionMatrix();

      glm::vec4 cornersProjNorm[4];
      for (size_t i = 0; i < 4; i++) {
        cornersProjNorm[i] = viewProjection * glm::vec4(corners3[i], 1.0);
      }

      double w = _cam->getW();
      double h = _cam->getH();

      double left, top, right, bot, depth;

      left  = w * cornersProjNorm[0].x + 1./2;
      top   = h * cornersProjNorm[0].y + 1./2;
      right = w * cornersProjNorm[1].x + 1./2;
      bot   = h * cornersProjNorm[3].y + 1./2;
      depth = cornersProjNorm[3].z + 1./2;

      top = h-top;
      bot = h-bot;

      sf::IntRect cornersRect((int) left, (int) top, (int) right-left, (int) bot-top);

      _e[i]->set2DCorners(cornersRect);
      _e[i]->setDepth(depth);

      _vis.insert(_e[i]);
    }
  }
}

void Game::render() const {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Skybox

  /*glEnable(GL_TEXTURE_CUBE_MAP_ARB);
  glDepthMask(GL_FALSE);
  _skybox->draw();
  glDepthMask(GL_TRUE);
  glDisable(GL_TEXTURE_CUBE_MAP_ARB);*/

  // Heightmap

  glUseProgram(_hmapShader.getProgramID());
  glActiveTexture(GL_TEXTURE0);
  glUniform1i(glGetUniformLocation(_hmapShader.getProgramID(), "tex"), 0);

  for(auto it = _terrain.begin() ; it != _terrain.end() ; ++it) {
    if (it->second->isVisible()) {
      glm::mat4 modelview = glm::translate(glm::mat4(1.f),
        glm::vec3(CHUNK_SIZE * it->first.x, 0.0f, CHUNK_SIZE * it->first.y)
      );

      _hmapShader.sendModelMatrix(_cam, modelview);

      it->second->draw();

    }
  }

  glUseProgram(0);

  // igElements

  // glEnable (GL_BLEND);
  // glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  for (auto it = _vis.begin() ; it != _vis.end() ; ++it) {
    _hmapShader.sendModelMatrix(_cam, glm::mat4(1.f));
    (*it)->draw();
  }

  // glDisable(GL_BLEND);
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
  _e.push_back(new Lion(get2DCoord(screenTarget), AnimationManager(_lionTex, "res/animals/lion/animInfo.xml")));
}

void Game::generateHerd(sf::Vector2f pos, int count) {
  srand(time(NULL));
  double r, theta;
  sf::Vector2<double> p, diff;
  bool add;

  std::vector<Antilope*> tmp;

  for (int i = 0 ; i < count ; i++) {
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
      tmp.push_back(new Antilope(p, AnimationManager(_antilopeTex, "res/animals/antilope/animInfo.xml")));
    }
  }

  for (int i = 0 ; i < count ; i++) {
    _e.push_back(tmp[i]);
  }
}

void Game::generateForests(sf::Vector2i pos) {
  srand(time(NULL));
  double r, theta;
  sf::Vector2<double> p, diff;
  bool add;
  int count, nbTrees;

  std::vector<Tree*> tmp;

  std::vector<Center*> centers = _map->getCentersInChunk(pos);

  for (unsigned int i = 0 ; i < centers.size() ; i++) {
    if (centers[i]->biome >= 11) { // No forests in other biomes
      count = 0;
      tmp.clear();
      nbTrees = _treeTexManager.getExtension(centers[i]->biome);
      nbTrees *= 1.5;

      for (int j = 0 ; j < nbTrees ; j++) {
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

      for (int j = 0 ; j < count ; j++) {
        _e.push_back(tmp[j]);
      }
    }
  }
}

sf::Vector2<double> Game::get2DCoord(sf::Vector2i screenTarget) const {
  screenTarget.y = _cam->getH() - screenTarget.y; // Inverted coordinates

  GLfloat d;
  glReadPixels(screenTarget.x, screenTarget.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &d);

  glm::vec4 screenCoord(screenTarget.x, screenTarget.y, d, 1.0);

  glm::vec4 modelCoord = glm::inverse(_cam->getViewProjectionMatrix()) * screenCoord;

  return sf::Vector2<double>(modelCoord.x,modelCoord.z);
}
