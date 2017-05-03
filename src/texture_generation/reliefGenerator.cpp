#include "reliefGenerator.h"

#include <cmath>
#include <iostream>

#include "perlin.h"

ReliefGenerator::ReliefGenerator (const TerrainGeometry& terrainGeometry) :
  _terrainGeometry(terrainGeometry) {}

void ReliefGenerator::fillIslandMask(size_t size) {
  std::vector<float> islandMask(size*size);

  for (size_t i = 0; i < size; i++) {
    for (size_t j = 0; j < size; j++) {
      islandMask[i*size + j] = _terrainGeometry.getBiome(sf::Vector2f(
        i * MAX_COORD / size, j * MAX_COORD / size), 0) == OCEAN ? 0 : 1;
    }
  }

  _islandMask.setPixels(islandMask);
}

void ReliefGenerator::generateRelief(size_t size) {
  fillIslandMask(size);
  _islandMask.smoothDilatation(50);

  Perlin perlinRelief(3, 0.06, 0.75, size);
  _relief.setPixels(perlinRelief.getPixels());
  _relief.multiply(_islandMask.getPixels());
}
