#include "contentGenerator.h"

#include <cstdlib>
#include <sstream>

#include "antilope.h"
#include "lion.h"
#include "tree.h"

#define CONTENT_RES 512

#define MIN_ANTILOPE_PROX 5.f
#define HERD_RADIUS 10.f // for 10 antilopes

ContentGenerator::ContentGenerator(Map& map) :
  _map(map),
  _perlinGenerator(0, CONTENT_RES),
  _treesInChunk(NB_CHUNKS * NB_CHUNKS) {

  _perlinGenerator.setParams(3, 0.06, 0.75);

  std::vector<bool> initForestsMask(CONTENT_RES,false);
  _forestsMask = std::vector<std::vector<bool> >(CONTENT_RES, initForestsMask);
}

void ContentGenerator::init() {
  _antilopeTexManager.load("res/animals/antilope/");
  _lionTexManager.    load("res/animals/lion/");
  _treeTexManager.    load("res/trees/");

  for (int i = 0 ; i < CONTENT_RES ; i++) {
		for (int j = 0 ; j < CONTENT_RES ; j++) {
      if (_perlinGenerator.getValue(i,j) > 0.6)
			  _forestsMask[i][j] = true;
		}
	}
}

void ContentGenerator::saveToImage(std::string savename) {
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

bool ContentGenerator::isInForestMask(sf::Vector2f pos) {
  return _forestsMask[(int) (pos.x / MAX_COORD * CONTENT_RES)]
                     [(int) (pos.y / MAX_COORD * CONTENT_RES)];
}

bool ContentGenerator::notTooCloseToOtherTrees(sf::Vector2f pos, float distance) {
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

std::vector<igElement*> ContentGenerator::genForestsInChunk(size_t x, size_t y) {
  sf::Vector2f chunkPos(x*CHUNK_SIZE, y*CHUNK_SIZE);
  std::vector<igElement*> res;

  // _treesInChunk[x*CHUNK_SIZE + y].reserve(100);

  for (size_t i = 0; i < 400; i++) {
    sf::Vector2f pos(RANDOMF * CHUNK_SIZE, RANDOMF * CHUNK_SIZE);
    pos += chunkPos;

    Biome biome = _map.getClosestCenter(pos)->biome;

    if (biome >= 11) { // No forests in other biomes
      if (isInForestMask(pos)) {
        if (notTooCloseToOtherTrees(pos, _treeTexManager.getDensity(biome))) {
          _treesInChunk[x*NB_CHUNKS + y].push_back(pos);

          res.push_back(new Tree(pos, _treeTexManager, biome,
            (int) ((RANDOMF - 0.01f) * _treeTexManager.getNBTrees(biome))));
        }
      }
    }
  }

  return res;
}

std::vector<igElement*> ContentGenerator::genHerd(sf::Vector2f pos, size_t count) {
  float r, theta;
  sf::Vector2f p, diff;
  bool add;

  std::vector<igElement*> res;

  for (size_t i = 0 ; i < count ; i++) {
    add = true;
    r = sqrt(RANDOMF) * HERD_RADIUS * sqrt(count);
    theta = RANDOMF * 2*M_PI;

    p.x = pos.x + r*cos(theta);
    p.y = pos.y + r*sin(theta);

    for (unsigned int j = 0 ; j < res.size() ; j++) {
      diff = res[j]->getPos() - p;

      if (diff.x * diff.x + diff.y * diff.y < MIN_ANTILOPE_PROX) {
        i--;
        add = false;
      }
    }

    if (add) {
      res.push_back(new Antilope(p, AnimationManager(_antilopeTexManager)));
    }
  }

  return res;
}

std::vector<igElement*> ContentGenerator::genLion(sf::Vector2f pos) {
  std::vector<igElement*> res;
  res.push_back(new Lion(pos, AnimationManager(_lionTexManager)));
  return res;
}
