#include "game.h"
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>

#define MIN_ANTILOPE_PROX 5.f
#define HERD_RADIUS 10.f // for 10 antilopes
#define CHUNK_BEGIN_X 30
#define CHUNK_BEGIN_Y 11

Game::Game(Camera* camera, Map* _map) :
  cam(camera),
  map(_map),
  terrainTexManager("res/terrain/"),
  treeTexManager("res/trees/"),
  hmapShader("src/shaders/heightmap.vert", "src/shaders/heightmap.frag") {
  hmapShader.load();

  cam->translate(CHUNK_BEGIN_Y*CHUNK_SIZE,CHUNK_BEGIN_X*CHUNK_SIZE);

  Heightmap* hmap[4];

  srand(time(NULL));

  hmap[0] = new Heightmap(sf::Vector2i(CHUNK_BEGIN_X,CHUNK_BEGIN_Y), rand(), &terrainTexManager, map);
  hmap[1] = new Heightmap(sf::Vector2i(CHUNK_BEGIN_X,CHUNK_BEGIN_Y - 1), rand(), &terrainTexManager, map);
  hmap[2] = new Heightmap(sf::Vector2i(CHUNK_BEGIN_X - 1,CHUNK_BEGIN_Y), rand(), &terrainTexManager, map);
  hmap[3] = new Heightmap(sf::Vector2i(CHUNK_BEGIN_X - 1,CHUNK_BEGIN_Y - 1), rand(), &terrainTexManager, map);

  hmap[0]->generate(std::vector<Constraint>());
  terrain.insert(std::pair<sf::Vector2i, Chunk*>(hmap[0]->getChunkPos(), hmap[0]));
  terrainBorder.insert(hmap[0]->getChunkPos());

  for (int i = 1 ; i < 4 ; i++) {
    std::vector<Constraint> c;

    for (int j = 0 ; j < i ; j++)
      c.push_back(hmap[j]->getConstraint(hmap[i]->getChunkPos()));

    hmap[i]->generate(c);
    terrain.insert(std::pair<sf::Vector2i, Chunk*>(hmap[i]->getChunkPos(), hmap[i]));
    terrainBorder.insert(hmap[i]->getChunkPos());
    generateForests(hmap[i]->getChunkPos());
  }

  skybox = new Skybox("res/skybox/skybox", cam);

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

    lionTex.push_back(curTex);
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

    antilopeTex.push_back(curTex);
  }

  generateHerd(sf::Vector2f(CHUNK_BEGIN_X * CHUNK_SIZE, CHUNK_BEGIN_Y * CHUNK_SIZE), 20);
}

Game::~Game() {
	for(auto it = terrain.begin() ; it != terrain.end() ; ++it) {
      //delete it->second;
  }

  delete skybox;
  for (unsigned int i = 0 ; i < e.size() ; i++) {
    delete e[i];
  }
}

void Game::generateHeightmap(sf::Vector2i pos) {
  sf::Vector2i tmp;
  std::vector<Constraint> c;

  for (int j = 0 ; j < 4 ; j++) { // Get constraints
    tmp = neighbour(pos,j);
    std::map<sf::Vector2i, Chunk*>::iterator it = terrain.find(tmp);

    if (it != terrain.end()) {
      c.push_back(it->second->getConstraint(pos));
    }
  }

  Heightmap* hmap = new Heightmap(pos, rand(), &terrainTexManager, map);
  hmap->generate(c);
  terrain.insert(std::pair<sf::Vector2i, Chunk*>(hmap->getChunkPos(), hmap));
  terrainBorder.insert(hmap->getChunkPos());
}

void Game::generateNeighbourChunks(sf::Vector2i pos) {
  if (terrainBorder.find(pos) != terrainBorder.end()) {
    sf::Vector2i tmp;
    for (int i = 0; i < 4; i++) {
      tmp = neighbour(pos,i);

      if (terrain.find(tmp) == terrain.end()) { // We only add the neighbour if it's not already generated
        // If the chunk to be generated is not handled by the map, we generate an ocean
        if (tmp.x < 0 || tmp.x >= map->getNbChunks() || tmp.y < 0 || tmp.y >= map->getNbChunks()) {
          terrain.insert(std::pair<sf::Vector2i, Chunk*>(tmp, new Ocean(tmp, terrainTexManager.getTexIndex(OCEAN))));
          terrainBorder.insert(tmp);
        }

        else if (map->getClosestCenter(sf::Vector2<double>(tmp.x * CHUNK_SIZE, tmp.y * CHUNK_SIZE))->biome == OCEAN &&
                 map->getClosestCenter(sf::Vector2<double>(tmp.x * CHUNK_SIZE, (tmp.y+1) * CHUNK_SIZE))->biome == OCEAN &&
                 map->getClosestCenter(sf::Vector2<double>((tmp.x+1) * CHUNK_SIZE, tmp.y * CHUNK_SIZE))->biome == OCEAN &&
                 map->getClosestCenter(sf::Vector2<double>((tmp.x+1) * CHUNK_SIZE, (tmp.y+1) * CHUNK_SIZE))->biome == OCEAN) {

          terrain.insert(std::pair<sf::Vector2i, Chunk*>(tmp, new Ocean(tmp, terrainTexManager.getTexIndex(OCEAN))));
          terrainBorder.insert(tmp);
        }

        else {
          generateHeightmap(tmp);
          generateForests(tmp);
        }
      }
    }

    terrainBorder.erase(pos);
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
  int camPosX = cam->getPointedPos().x < 0 ? cam->getPointedPos().x / CHUNK_SIZE - 1 : cam->getPointedPos().x / CHUNK_SIZE;
  int camPosY = cam->getPointedPos().y < 0 ? cam->getPointedPos().y / CHUNK_SIZE - 1 : cam->getPointedPos().y / CHUNK_SIZE;
  cam->setHeight( terrain[sf::Vector2i(camPosX, camPosY)]
                          ->getHeight(cam->getPointedPos().x - CHUNK_SIZE * camPosX,
                                      cam->getPointedPos().y - CHUNK_SIZE * camPosY));

  for (auto it = terrainBorder.begin() ; it != terrainBorder.end() ; ++it) {
      if (terrain[(*it)]->isVisible())
          generateNeighbourChunks(*it);
  }

  for (unsigned int i = 0 ; i < e.size() ; i++) {
      if (e[i]->getAbstractType() != igE) {
          igMovingElement* igM = (igMovingElement*) e[i];
          if (igM->getMovingType() == PREY) {
              Antilope* atlp = (Antilope*) igM;
              atlp->updateState(e);
          }

          else if (igM->getMovingType() == HUNTER) {
              Lion* lion = (Lion*) igM;
              lion->kill(e);
          }
      }
  }

  for (auto it = terrain.begin() ; it != terrain.end() ; ++it) {
      it->second->calculateFrustum(cam);
  }

  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT,viewport);
  GLdouble projection[16];
  glGetDoublev(GL_PROJECTION_MATRIX,projection);
  GLdouble modelview[16];
  glGetDoublev(GL_MODELVIEW_MATRIX,modelview);

  vis.clear();

  for (unsigned int i = 0 ; i < e.size() ; i++) {
    int chunkPosX = e[i]->getPos().x / CHUNK_SIZE;
    int chunkPosY = e[i]->getPos().y / CHUNK_SIZE;

    if (terrain.find(sf::Vector2i(chunkPosX, chunkPosY)) != terrain.end() &&
        terrain.at(sf::Vector2i(chunkPosX, chunkPosY))->isVisible()) {

      //std::cout << chunkPosX << " " << chunkPosY << std::endl;

      e[i]->update(elapsed, cam->getTheta()); // Choose the right sprite and update pos

      chunkPosX = e[i]->getPos().x / CHUNK_SIZE;
      chunkPosY = e[i]->getPos().y / CHUNK_SIZE;

      // No test yet to see if the element can move to its new pos (no collision)
      double newHeight =   terrain[sf::Vector2i(chunkPosX, chunkPosY)]
                           ->getHeight(e[i]->getPos().x - (int) CHUNK_SIZE * chunkPosX,
                                       e[i]->getPos().y - (int) CHUNK_SIZE * chunkPosY);

      // Calculate new corners
      sf::Vector3f corners3[4];

      float width = e[i]->getW();

      corners3[0] = sf::Vector3f( e[i]->getPos().x + sin(cam->getTheta()*M_PI/180.)*width/2,
                                  newHeight + e[i]->getH(),
                                  e[i]->getPos().y - cos(cam->getTheta()*M_PI/180.)*width/2);


      corners3[1] = sf::Vector3f( e[i]->getPos().x - sin(cam->getTheta()*M_PI/180.)*width/2,
                                  newHeight + e[i]->getH(),
                                  e[i]->getPos().y + cos(cam->getTheta()*M_PI/180.)*width/2);


      corners3[2] = sf::Vector3f( e[i]->getPos().x - sin(cam->getTheta()*M_PI/180.)*width/2,
                                  newHeight,
                                  e[i]->getPos().y + cos(cam->getTheta()*M_PI/180.)*width/2);


      corners3[3] = sf::Vector3f( e[i]->getPos().x + sin(cam->getTheta()*M_PI/180.)*width/2,
                                  newHeight,
                                  e[i]->getPos().y - cos(cam->getTheta()*M_PI/180.)*width/2);

      e[i]->set3DCorners(corners3);

      // Calculate their projections

      GLdouble left, top, right, bot, depth;
      GLdouble trash;

      gluProject(corners3[0].x,corners3[0].y,corners3[0].z,modelview,projection,viewport,&left,  &top,   &trash);
      gluProject(corners3[1].x,corners3[1].y,corners3[1].z,modelview,projection,viewport,&right, &trash, &trash);
      gluProject(corners3[3].x,corners3[3].y,corners3[3].z,modelview,projection,viewport,&trash, &bot,   &depth);

      top = viewport[3]-top;
      bot = viewport[3]-bot;

      sf::IntRect cornersRect((int) left, (int) top, (int) right-left, (int) bot-top);

      e[i]->set2DCorners(cornersRect);
      e[i]->setDepth(depth);

      vis.insert(e[i]);
    }
  }
}

void Game::render() const {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Skybox

  /*glEnable(GL_TEXTURE_CUBE_MAP_ARB);
  glDepthMask(GL_FALSE);
  skybox->draw();
  glDepthMask(GL_TRUE);
  glDisable(GL_TEXTURE_CUBE_MAP_ARB);*/

  // Heightmap

  glUseProgram(hmapShader.getProgramID());
  glActiveTexture(GL_TEXTURE0);
  glUniform1i(glGetUniformLocation(hmapShader.getProgramID(), "tex"), 0);
  glUniform3f(glGetUniformLocation(hmapShader.getProgramID(), "camPos"), cam->getPos().x, cam->getPos().y, cam->getPos().z);

  for(auto it = terrain.begin() ; it != terrain.end() ; ++it) {
    if (it->second->isVisible()) {
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();

      glTranslatef(CHUNK_SIZE * it->first.x, 0.0f, CHUNK_SIZE * it->first.y);

      it->second->draw();


      glPopMatrix();
    }
  }

  glUseProgram(0);

  // igElements

  glEnable (GL_TEXTURE_2D);
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //std::cout << e.size() << " " << vis.size() << std::endl;

  for (auto it = vis.begin() ; it != vis.end() ; ++it) {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    (*it)->draw();
    glPopMatrix();
  }

  glDisable(GL_BLEND | GL_TEXTURE_2D);
}

void Game::select(sf::IntRect rect, bool add) {
  if (!add)
    sel.clear();

  for (unsigned int i = 0 ; i < e.size() ; i++) {
    if (sel.find(e[i]) == sel.end()) { // e[i] is not selected yet, we can bother to calculate
      sf::IntRect c = e[i]->get2DCorners();

      int centerX, centerY;

      centerX = c.left + c.width / 2;
      centerY = c.top + c.height / 2;

      if (rect.contains(centerX, centerY)) {
        if (e[i]->getAbstractType() == CTRL)
          sel.insert(e[i]);
      }

      else if (   c.contains(rect.left, rect.top) ||
                  c.contains(rect.left + rect.width, rect.top) ||
                  c.contains(rect.left + rect.width, rect.top + rect.height) ||
                  c.contains(rect.left, rect.top + rect.height)  ) {
        if (e[i]->getAbstractType() == CTRL)
          sel.insert(e[i]);
      }
    }
  }
}

void Game::moveSelection(sf::Vector2i screenTarget) {
  sf::Vector2<double> target = get2DCoord(screenTarget);

  for(auto it = sel.begin(); it != sel.end(); ++it) {
    if ((*it)->getAbstractType() == CTRL) {
      Controllable* tmp = (Controllable*) *it;
      tmp->setTarget(target);
    }
  }
}

void Game::moveCamera(sf::Vector2f newAimedPos) {
  generateHeightmap(sf::Vector2i(newAimedPos.x / CHUNK_SIZE, newAimedPos.y / CHUNK_SIZE));
  cam->setPointedPos(newAimedPos);
}

void Game::addLion(sf::Vector2i screenTarget) {
  e.push_back(new Lion(get2DCoord(screenTarget), AnimationManager(lionTex, "res/animals/lion/animInfo.xml")));
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
      tmp.push_back(new Antilope(p, AnimationManager(antilopeTex, "res/animals/antilope/animInfo.xml")));
    }
  }

  for (int i = 0 ; i < count ; i++) {
    e.push_back(tmp[i]);
  }
}

void Game::generateForests(sf::Vector2i pos) {
  srand(time(NULL));
  double r, theta;
  sf::Vector2<double> p, diff;
  bool add;
  int count, nbTrees;

  std::vector<Tree*> tmp;

  std::vector<Center*> centers = map->getCentersInChunk(pos);

  for (unsigned int i = 0 ; i < centers.size() ; i++) {
    if (centers[i]->biome >= 11) { // No forests in other biomes
      count = 0;
      tmp.clear();
      nbTrees = treeTexManager.getExtension(centers[i]->biome);
      nbTrees *= 1.5;

      for (int j = 0 ; j < nbTrees ; j++) {
        add = true;
        r = sqrt(randomD()) * treeTexManager.getExtension(centers[i]->biome) * sqrt(nbTrees);
        theta = randomD() * 2*M_PI;

        p.x = centers[i]->x + r*cos(theta);
        p.y = centers[i]->y + r*sin(theta);

        for (unsigned int k = 0 ; k < tmp.size() ; k++) {
          diff = tmp[k]->getPos() - p;

          if (diff.x * diff.x + diff.y * diff.y < treeTexManager.getDensity(centers[i]->biome) ||
            map->getClosestCenter(p)->biome != centers[i]->biome) {
            add = false;
          }
        }

        if (add) {
          count++;
          tmp.push_back(new Tree(p, &treeTexManager, centers[i]->biome,
            (int) ((randomD() - 0.01f) * treeTexManager.getNBTrees(centers[i]->biome))));
        }
      }

      for (int j = 0 ; j < count ; j++) {
        e.push_back(tmp[j]);
      }
    }
  }
}

sf::Vector2<double> Game::get2DCoord(sf::Vector2i screenTarget) const {
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT,viewport);
  GLdouble projection[16];
  glGetDoublev(GL_PROJECTION_MATRIX,projection);
  GLdouble modelview[16];
  glGetDoublev(GL_MODELVIEW_MATRIX,modelview);
  GLdouble winX,winY;

  screenTarget.y=viewport[3]-screenTarget.y; // Inverted coordinates

  winX=(double)screenTarget.x;
  winY=(double)screenTarget.y;

  GLfloat d;
  glReadPixels(screenTarget.x, screenTarget.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &d);

  double x,y,z;
  gluUnProject(winX,winY,d,modelview,projection,viewport,&x,&y,&z);

  return sf::Vector2<double>(x,z);
}
