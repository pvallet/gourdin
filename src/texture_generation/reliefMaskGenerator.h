#pragma once

#include "generatedImage.h"
#include "terrainGeometry.h"

class ReliefMaskGenerator {
public:
  ReliefMaskGenerator (const TerrainGeometry& terrainGeometry);

  void generateMask(size_t size);
  // Morphologic dilatation of the black regions, with grayness depending on the distance to the black region
  void smoothDilatation(float radius);

  GeneratedImage getMask() const {return GeneratedImage(_islandMask);}

private:
  const TerrainGeometry& _terrainGeometry;

  size_t _maskSize;
  std::vector<float> _islandMask;
};
