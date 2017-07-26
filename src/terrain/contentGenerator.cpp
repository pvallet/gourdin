#include "contentGenerator.h"

#include <algorithm>
#include <cstdlib>
#include <sstream>

#include "antilope.h"
#include "lion.h"
#include "tree.h"

#define CONTENT_RES 512

ContentGenerator::ContentGenerator(const TerrainGeometry& terrainGeometry) :
  _terrainGeometry(terrainGeometry),
  _perlinGenerator(3, 0.06, 0.75, CONTENT_RES),
  _treesInChunk(NB_CHUNKS * NB_CHUNKS) {

  std::vector<bool> initForestsMask(CONTENT_RES,false);
  _forestsMask = std::vector<std::vector<bool> >(CONTENT_RES, initForestsMask);
}

void ContentGenerator::init() {
  _animManagerInits.resize(ANIMALS_NB_ITEMS);

  for (size_t i = 0; i < ANIMALS_NB_ITEMS; i++) {
    std::ostringstream path;
    path << "res/animals/" << i << '/';
    _animManagerInits[i].load(path.str());
  }

  _treeTexManager.load("res/trees/");

  for (int i = 0 ; i < CONTENT_RES ; i++) {
		for (int j = 0 ; j < CONTENT_RES ; j++) {
      if (_perlinGenerator.getValue(i,j) > 0.4)
			  _forestsMask[i][j] = true;
		}
	}
}

bool ContentGenerator::isInForestMask(glm::vec2 pos) const {
  return _forestsMask[(int) (pos.x / MAX_COORD * CONTENT_RES)]
                     [(int) (pos.y / MAX_COORD * CONTENT_RES)];
}

bool ContentGenerator::notTooCloseToOtherTrees(glm::vec2 pos, float distance) const {
  for (size_t i = std::max((int) ((pos.x - distance)/CHUNK_SIZE), 0);
              i < std::min((int) ((pos.x + distance)/CHUNK_SIZE), NB_CHUNKS); i++) {
  for (size_t j = std::max((int) ((pos.y - distance)/CHUNK_SIZE), 0);
              j < std::min((int) ((pos.y + distance)/CHUNK_SIZE), NB_CHUNKS); j++) {

    for (auto it = _treesInChunk[i*NB_CHUNKS +j].begin();
             it != _treesInChunk[i*NB_CHUNKS +j].end(); it++) {

      if (glm::length(pos - (*it)) < distance)
        return false;
    }
  }
  }

  return true;
}

struct compTrees {
  bool operator()(const igElement* lhs, const igElement* rhs) const {
    const Tree* tLHS = dynamic_cast<const Tree*>(lhs);
    const Tree* tRHS = dynamic_cast<const Tree*>(rhs);
    return tLHS->getBiome() < tRHS->getBiome();
  }
};

std::vector<igElement*> ContentGenerator::genForestsInChunk(size_t x, size_t y) {
  glm::vec2 chunkPos(x*CHUNK_SIZE, y*CHUNK_SIZE);
  std::vector<igElement*> res;

  for (size_t i = 0; i < 1500; i++) {
    glm::vec2 pos(RANDOMF * CHUNK_SIZE, RANDOMF * CHUNK_SIZE);
    pos += chunkPos;

    Biome biomeInPos = _terrainGeometry.getBiome(pos,1);

    if (biomeInPos != BIOME_NB_ITEMS) {
      if (biomeInPos >= 11) { // No forests in other biomes
        if (isInForestMask(pos)) {
          if (notTooCloseToOtherTrees(pos, _treeTexManager.getDensity(biomeInPos))) {
            _treesInChunk[x*NB_CHUNKS + y].push_back(pos);

            res.push_back(new Tree(pos, _treeTexManager, biomeInPos,
              (int) ((RANDOMF - 0.01f) * _treeTexManager.getNBTrees(biomeInPos))));
          }
        }
      }
    }
  }

  // Sorting reduces the number of glCalls to display all the trees
  std::sort(res.begin(), res.end(), compTrees());

  return res;
}

std::vector<igMovingElement*> ContentGenerator::genHerds() const {
  std::vector<igMovingElement*> res;

  for (size_t i = 0; i < 400; i++) {
    glm::vec2 pos(RANDOMF * MAX_COORD, RANDOMF * MAX_COORD);

    Biome biomeInPos = _terrainGeometry.getBiome(pos,1);

    if (biomeInPos == TEMPERATE_RAIN_FOREST ||
        biomeInPos == TEMPERATE_DECIDUOUS_FOREST ||
        biomeInPos == GRASSLAND) {
      std::vector<igMovingElement*> newItems = genHerd(pos, RANDOMF * 15 + 5, DEER);
      res.insert(res.end(), newItems.begin(), newItems.end());
    }

    else if (biomeInPos == TROPICAL_SEASONAL_FOREST) {
      std::vector<igMovingElement*> newItems = genHerd(pos, RANDOMF * 25 + 10, ANTILOPE);
      res.insert(res.end(), newItems.begin(), newItems.end());
    }
  }

  return res;
}

std::vector<glm::vec2> ContentGenerator::scatteredPositions(glm::vec2 center,
  size_t count, float radius, float minProximity) const {

  float r, theta;
  glm::vec2 p, diff;
  bool add;

  std::vector<glm::vec2> res;

  for (size_t i = 0 ; i < 2*count ; i++) {
    add = true;
    r = sqrt(RANDOMF) * radius * sqrt(count);
    theta = RANDOMF * 2*M_PI;

    p.x = center.x + r*cos(theta);
    p.y = center.y + r*sin(theta);

    for (unsigned int j = 0 ; j < res.size() ; j++) {
      diff = res[j] - p;

      if (diff.x * diff.x + diff.y * diff.y < minProximity)
        add = false;
    }

    if (_terrainGeometry.isWater(p,0))
      add = false;

    if (add) {
      res.push_back(p);

      if (res.size() == count)
        break;
    }
  }

  return res;
}

std::vector<igMovingElement*> ContentGenerator::genHerd(glm::vec2 pos, size_t count, Animals animal) const {
  std::vector<igMovingElement*> res;

  std::vector<glm::vec2> positions = scatteredPositions(pos, count, 10, 5);

  for (size_t i = 0; i < positions.size(); i++) {
    res.push_back(new Antilope(positions[i], AnimationManager(_animManagerInits[animal]), _terrainGeometry));
  }

  return res;
}

std::vector<igMovingElement*> ContentGenerator::genTribe(glm::vec2 pos) const {
  std::vector<igMovingElement*> res;

  std::vector<glm::vec2> positions = scatteredPositions(pos, RANDOMF * 5 + 5, 10, 5);

  for (size_t i = 0; i < positions.size(); i++) {
    float randNumber = RANDOMF;
    Animals animal;
    if (randNumber < 0.3)
      animal = AOE1_MAN;
    else if (randNumber < 0.5)
      animal = AOE2_MAN;
    else
      animal = WOMAN;
    res.push_back(new Controllable(positions[i], AnimationManager(_animManagerInits[animal]), _terrainGeometry));
  }

  return res;
}

std::vector<igMovingElement*> ContentGenerator::genLion(glm::vec2 pos) const {
  std::vector<igMovingElement*> res;
  Biome biomeInPos = _terrainGeometry.getBiome(pos,1);

  if (biomeInPos == TROPICAL_RAIN_FOREST)
    res.push_back(new Lion(pos, AnimationManager(_animManagerInits[LEOPARD]), _terrainGeometry));
  else if (biomeInPos == TROPICAL_SEASONAL_FOREST)
    res.push_back(new Lion(pos, AnimationManager(_animManagerInits[LION]), _terrainGeometry));

  else if (biomeInPos == TEMPERATE_RAIN_FOREST ||
           biomeInPos == TEMPERATE_DECIDUOUS_FOREST ||
           biomeInPos == GRASSLAND ||
           biomeInPos == TAIGA ||
           biomeInPos == SHRUBLAND ||
           biomeInPos == TUNDRA)
    res.push_back(new Lion(pos, AnimationManager(_animManagerInits[WOLF]), _terrainGeometry));

  return res;
}
