#pragma once

#include "generatedImage.h"
#include "terrainGeometry.h"

class ReliefGenerator {
public:
  ReliefGenerator (const TerrainGeometry& terrainGeometry);

  // Run all steps to have the final relief image
  void generateRelief(size_t size);

  inline void saveToFile(std::string filename) const {_relief.saveToFile(filename);}
  inline const GeneratedImage& getRelief() const {return _relief;}

  inline const GeneratedImage& getIslandMask() const {return _islandMask;}
  inline const GeneratedImage& getElevationMask() const {return _elevationMask;}

private:
  void convertMapData(size_t size);
  // Average heights of contiguous pixels of a lake and feeds it to _lakesElevations
  static std::vector<float> computeLakesElevations(
    size_t size, const TerrainGeometry::SubdivisionLevel& smoother, float baseColor);

  const TerrainGeometry& _terrainGeometry;

  GeneratedImage _lakesElevations;

  GeneratedImage _elevationMask;
  GeneratedImage _islandMask;
  GeneratedImage _lakesMask;
  GeneratedImage _relief;
};
