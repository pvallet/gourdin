#pragma once

#include <SFML/Graphics.hpp>

#include <list>

#include "terrainGeometry.h"

#include "animationManagerInitializer.h"
#include "treeTexManager.h"

#include "perlin.h"
#include "utils.h"

class igElement;

class ContentGenerator {
public:
  ContentGenerator(const TerrainGeometry& terrainGeometry);

  void init();

  void saveToImage(std::string savename) const;

  std::vector<igElement*> genForestsInChunk(size_t x, size_t y);
  // std::vector<igElement*> genHerdsInChunk(size_t x, size_t y);
  std::vector<igElement*> genHerd(sf::Vector2f pos, size_t count) const;

  std::vector<igElement*> genLion(sf::Vector2f pos) const;

private:
  bool isInForestMask(sf::Vector2f pos) const;
  bool notTooCloseToOtherTrees(sf::Vector2f pos, float distance) const;

  const TerrainGeometry& _terrainGeometry;
  Perlin _perlinGenerator;

  std::vector<std::vector<bool> > _forestsMask;

  // Contains the trees that are in a given Chunk
	// The chunks are sorted as chunk.x * NB_CHUNKS + chunk.y
  std::vector<std::list<sf::Vector2f> > _treesInChunk;

  AnimationManagerInitializer _antilopeTexManager;
  AnimationManagerInitializer _lionTexManager;
  TreeTexManager _treeTexManager;
};