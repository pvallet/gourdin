#include "contentGenerator.h"

#include <cstdlib>
#include <sstream>

#include "antilope.h"
#include "human.h"
#include "lion.h"
#include "tree.h"

#define CONTENT_RES 512

ContentGenerator::ContentGenerator(const TerrainGeometry& terrainGeometry) :
  _terrainGeometry(terrainGeometry),
  _perlinGenerator(0, CONTENT_RES),
  _treesInChunk(NB_CHUNKS * NB_CHUNKS) {

  _perlinGenerator.setParams(3, 0.06, 0.75);

  std::vector<bool> initForestsMask(CONTENT_RES,false);
  _forestsMask = std::vector<std::vector<bool> >(CONTENT_RES, initForestsMask);
}

void ContentGenerator::init() {
  _animManagerInits.resize(NB_ANIMALS);

  for (size_t i = 0; i < NB_ANIMALS; i++) {
    std::ostringstream path;
    path << "res/animals/" << i << '/';
    _animManagerInits[i].load(path.str());
  }

  _treeTexManager.load("res/trees/");

  for (int i = 0 ; i < CONTENT_RES ; i++) {
		for (int j = 0 ; j < CONTENT_RES ; j++) {
      if (_perlinGenerator.getValue(i,j) > 0.6)
			  _forestsMask[i][j] = true;
		}
	}
}

void ContentGenerator::saveToImage(std::string savename) const {
  std::vector<sf::Uint8> pixels(CONTENT_RES * CONTENT_RES * 4, 255);

	for (int i = 0 ; i < CONTENT_RES ; i++) { // Convert mask to array of pixels
		for (int j = 0 ; j < CONTENT_RES ; j++) {
      if (_forestsMask[i][j]) {
        pixels[i*4*CONTENT_RES + j*4] = 0;
        pixels[i*4*CONTENT_RES + j*4 + 2] = 0;
      }
		}
	}

	sf::Texture texture;
	texture.create(CONTENT_RES, CONTENT_RES);
	texture.update(&pixels[0]);

  std::ostringstream convert;
  convert << savename << ".png";

	texture.copyToImage().saveToFile(convert.str());
}

bool ContentGenerator::isInForestMask(sf::Vector2f pos) const {
  return _forestsMask[(int) (pos.x / MAX_COORD * CONTENT_RES)]
                     [(int) (pos.y / MAX_COORD * CONTENT_RES)];
}

bool ContentGenerator::notTooCloseToOtherTrees(sf::Vector2f pos, float distance) const {
  for (size_t i = std::max((int) ((pos.x - distance)/CHUNK_SIZE), 0);
              i < std::min((int) ((pos.x + distance)/CHUNK_SIZE), NB_CHUNKS); i++) {
  for (size_t j = std::max((int) ((pos.y - distance)/CHUNK_SIZE), 0);
              j < std::min((int) ((pos.y + distance)/CHUNK_SIZE), NB_CHUNKS); j++) {

    for (auto it = _treesInChunk[i*NB_CHUNKS +j].begin();
             it != _treesInChunk[i*NB_CHUNKS +j].end(); it++) {

      if (vu::norm(pos - (*it)) < distance)
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
  sf::Vector2f chunkPos(x*CHUNK_SIZE, y*CHUNK_SIZE);
  std::vector<igElement*> res;

  // _treesInChunk[x*CHUNK_SIZE + y].reserve(100);

  for (size_t i = 0; i < 400; i++) {
    sf::Vector2f pos(RANDOMF * CHUNK_SIZE, RANDOMF * CHUNK_SIZE);
    pos += chunkPos;

    Biome biomeInPos = _terrainGeometry.getBiome(pos,1);

    if (biomeInPos != NO_DEFINED_BIOME) {
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

std::vector<sf::Vector2f> ContentGenerator::scatteredPositions(sf::Vector2f center,
  size_t count, float radius, float minProximity) const {

  float r, theta;
  sf::Vector2f p, diff;
  bool add;

  std::vector<sf::Vector2f> res;

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

    if (add) {
      res.push_back(p);

      if (res.size() == count)
        break;
    }
  }

  return res;
}

std::vector<igMovingElement*> ContentGenerator::genHerd(sf::Vector2f pos, size_t count) const {
  std::vector<igMovingElement*> res;

  std::vector<sf::Vector2f> positions = scatteredPositions(pos, count, 10, 5);

  for (size_t i = 0; i < positions.size(); i++) {
    res.push_back(new Antilope(positions[i], AnimationManager(_animManagerInits[ANTILOPE])));
  }

  return res;
}

std::vector<igMovingElement*> ContentGenerator::genTribe(sf::Vector2f pos) const {
  std::vector<igMovingElement*> res;

  std::vector<sf::Vector2f> positions = scatteredPositions(pos, RANDOMF * 5 + 5, 10, 5);

  for (size_t i = 0; i < positions.size(); i++) {
    float randNumber = RANDOMF;
    Animals animal;
    if (randNumber < 0.3)
      animal = AOE1_MAN;
    else if (randNumber < 0.5)
      animal = AOE2_MAN;
    else
      animal = WOMAN;
    res.push_back(new Human(positions[i], AnimationManager(_animManagerInits[animal])));
  }

  return res;
}

std::vector<igMovingElement*> ContentGenerator::genLion(sf::Vector2f pos) const {
  std::vector<igMovingElement*> res;
  res.push_back(new Lion(pos, AnimationManager(_animManagerInits[LION])));
  return res;
}
