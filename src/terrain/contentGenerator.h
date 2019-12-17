#pragma once

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
  std::vector<igMovingElement*> genHerd(glm::vec2 pos, size_t count, Animals animal) const;
  std::vector<igMovingElement*> genTribe(glm::vec2 pos) const;
  std::vector<igMovingElement*> genLion(glm::vec2 pos) const;
  std::vector<igMovingElement*> genAntilope(glm::vec2 pos) const;

  const std::vector<std::vector<bool> >& getForestsMask() const {return _forestsMask;}

private:
  bool isInForestMask(glm::vec2 pos) const;
  bool notTooCloseToOtherTrees(glm::vec2 pos, float distance) const;
  // Generates randomly count positions around center within a radius radius.
  // Two individuals cannot be closer than minProximity and cannot be on water.
  // The function might return less than count positions if they are not suitable
  std::vector<glm::vec2> scatteredPositions(glm::vec2 center,
    size_t count, float radius, float minProximity) const;

  const TerrainGeometry& _terrainGeometry;
  Perlin _perlinGenerator;

  std::vector<std::vector<bool> > _forestsMask;

  // Contains the trees that are in a given Chunk
	// The chunks are sorted as chunk.x * NB_CHUNKS + chunk.y
  std::vector<std::list<glm::vec2> > _treesInChunk;

  std::vector<AnimationManagerInitializer> _animManagerInits;
  TreeTexManager _treeTexManager;
};
