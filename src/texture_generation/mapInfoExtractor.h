#pragma once

#include "terrainGeometry.h"
#include "utils.h"

#include <map>
#include <set>

class MapInfoExtractor {
public:
  MapInfoExtractor (const TerrainGeometry& terrainGeometry);

  void convertMapData(size_t size);
  void generateBiomesTransitions(float transitionSize);
  // Given an image per biome, fusion them according to the biomes and their transitions
  GeneratedImage imageFusion(const std::array<const GeneratedImage*, BIOME_NB_ITEMS>& imgs) const;

  inline size_t getSize() const {return _size;}

  inline const GeneratedImage& getIslandMask() const {return _islandMask;}
  inline const GeneratedImage& getLakesMask() const {return _lakesMask;}
  inline const GeneratedImage& getLakesElevations() const {return _lakesElevations;}
  inline const GeneratedImage& getElevationMask() const {return _elevationMask;}

private:
  // Average heights of contiguous pixels of a lake and feeds it to _lakesElevations
  static std::vector<float> computeLakesElevations(
    size_t size, const TerrainGeometry::SubdivisionLevel& smoother, float baseColor);

  // Recursive functions to compute lakes elevations
  // They help simplify lakes adjacency to point always to the lowest one
  // (e.g. if lake 2 is lake 1 and lake 3 is lake 2, put lake 3 is lake 1)
  static size_t getLowestIndexAdjacentLake(const std::vector<std::set<size_t> >& adjacentLakes,
    std::set<size_t>& nodesSeen, size_t node);


  const TerrainGeometry& _terrainGeometry;

  size_t _size;

  std::vector<Biome> _biomes;
  // For each pixel, contains each biome it belongs to and to which amount :
  // 1 if the pixel is inside the biome
  // min(0, 1 - distance_to_biome / transition_size) otherwise
  std::vector<std::map<Biome, float> > _biomesMasks;

  GeneratedImage _lakesElevations;
  GeneratedImage _elevationMask;
  GeneratedImage _islandMask;
  GeneratedImage _lakesMask;
};
