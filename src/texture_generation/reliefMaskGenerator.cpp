#include "reliefMaskGenerator.h"

ReliefMaskGenerator::ReliefMaskGenerator (const TerrainGeometry& terrainGeometry) :
  _terrainGeometry(terrainGeometry) {}

GeneratedImage ReliefMaskGenerator::generateMask(size_t size) const {
  std::vector<float> islandMask(size*size);

  for (size_t i = 0; i < size; i++) {
    for (size_t j = 0; j < size; j++) {
      islandMask[i*size + j] = _terrainGeometry.getBiome(sf::Vector2f(
        i * MAX_COORD / size, j * MAX_COORD / size), 0) == OCEAN ? 0 : 1;
    }
  }

  return GeneratedImage(islandMask);
}
