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

private:
  void fillIslandMask(size_t size);

  const TerrainGeometry& _terrainGeometry;

  GeneratedImage _islandMask;
  GeneratedImage _relief;
};
