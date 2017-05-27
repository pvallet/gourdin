#pragma once

#include <SFML/Graphics.hpp>

#include <list>

#include "terrainGeometry.h"

#include "animationManagerInitializer.h"
#include "treeTexManager.h"

#include "perlin.h"
#include "utils.h"

class igElement;
class igMovingElement;

class ContentGenerator {
public:
  ContentGenerator(const TerrainGeometry& terrainGeometry);

  void init();

  void saveToImage(std::string savename) const;

  std::vector<igElement*> genForestsInChunk(size_t x, size_t y);
  std::vector<igMovingElement*> genHerds() const;
  std::vector<igMovingElement*> genHerd(sf::Vector2f pos, size_t count, Animals animal) const;
  std::vector<igMovingElement*> genTribe(sf::Vector2f pos) const;
  std::vector<igMovingElement*> genLion(sf::Vector2f pos) const;

  const std::vector<std::vector<bool> >& getForestsMask() const {return _forestsMask;}

private:
  bool isInForestMask(sf::Vector2f pos) const;
  bool notTooCloseToOtherTrees(sf::Vector2f pos, float distance) const;
  std::vector<sf::Vector2f> scatteredPositions(sf::Vector2f center,
    size_t count, float radius, float minProximity) const;

  const TerrainGeometry& _terrainGeometry;
  Perlin _perlinGenerator;

  std::vector<std::vector<bool> > _forestsMask;

  // Contains the trees that are in a given Chunk
	// The chunks are sorted as chunk.x * NB_CHUNKS + chunk.y
  std::vector<std::list<sf::Vector2f> > _treesInChunk;

  std::vector<AnimationManagerInitializer> _animManagerInits;
  TreeTexManager _treeTexManager;
};
