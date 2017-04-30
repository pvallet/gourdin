#pragma once

#include "generatedImage.h"
#include "terrainGeometry.h"

class ReliefMaskGenerator {
public:
  ReliefMaskGenerator (const TerrainGeometry& terrainGeometry);

  GeneratedImage generateMask(size_t size) const;

private:
  const TerrainGeometry& _terrainGeometry;
};
