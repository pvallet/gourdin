#pragma once

#include <SFML/Graphics.hpp>

#include <list>

#include "map.h"

#include "animationManagerInitializer.h"
#include "treeTexManager.h"

#include "perlin.h"
#include "utils.h"

class igElement;

class ContentGenerator {
public:
  ContentGenerator(Map& map);

  void init();

  void saveToImage(std::string savename);

  std::vector<igElement*> genForestsInChunk(size_t x, size_t y);
  // std::vector<igElement*> genHerdsInChunk(size_t x, size_t y);
  std::vector<igElement*> genHerd(sf::Vector2f pos, size_t count);

  std::vector<igElement*> genLion(sf::Vector2f pos);

private:
  bool isInForestMask(sf::Vector2f pos);
  bool notTooCloseToOtherTrees(sf::Vector2f pos, float distance);

  Map& _map;
  Perlin _perlinGenerator;

  std::vector<std::vector<bool> > _forestsMask;

  // Contains the trees that are in a given Chunk
	// The chunks are sorted as chunk.x * NB_CHUNKS + chunk.y
  std::vector<std::list<sf::Vector2f> > _treesInChunk;

  AnimationManagerInitializer _antilopeTexManager;
  AnimationManagerInitializer _lionTexManager;
  TreeTexManager _treeTexManager;
};
