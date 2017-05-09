#pragma once

#include "terrainGeometry.h"

class MapInfoExtractor {
public:
  MapInfoExtractor (const TerrainGeometry& terrainGeometry);

  void convertMapData(size_t size);

  inline size_t getSize() const {return _size;}

  inline const GeneratedImage& getIslandMask() const {return _islandMask;}
  inline const GeneratedImage& getLakesMask() const {return _lakesMask;}
  inline const GeneratedImage& getLakesElevations() const {return _lakesElevations;}
  inline const GeneratedImage& getElevationMask() const {return _elevationMask;}

private:
  // Average heights of contiguous pixels of a lake and feeds it to _lakesElevations
  static std::vector<float> computeLakesElevations(
    size_t size, const TerrainGeometry::SubdivisionLevel& smoother, float baseColor);

  const TerrainGeometry& _terrainGeometry;

  size_t _size;

  GeneratedImage _lakesElevations;
  GeneratedImage _elevationMask;
  GeneratedImage _islandMask;
  GeneratedImage _lakesMask;
};
