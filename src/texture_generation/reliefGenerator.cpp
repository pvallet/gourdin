#include "reliefGenerator.h"

#include <cmath>
#include <iostream>

#include "perlin.h"

ReliefGenerator::ReliefGenerator (const TerrainGeometry& terrainGeometry) :
  _terrainGeometry(terrainGeometry) {}

void ReliefGenerator::convertMapData(size_t size) {
  std::vector<float> elevationMask(size*size);
  std::vector<float> islandMask(size*size);
  std::vector<float> lakesMask(size*size);

  float maxHeight = 0;

  for (size_t i = 0; i < size; i++) {
    for (size_t j = 0; j < size; j++) {
      sf::Vector2f pos(i * MAX_COORD / size, j * MAX_COORD / size);
      size_t index = i*size + j;
      Biome biome = _terrainGeometry.getBiome(pos, 0);

      elevationMask[index] = _terrainGeometry.getHeight(pos,0);
      if (elevationMask[index] > maxHeight)
        maxHeight = elevationMask[index];

      islandMask[index] = biome == OCEAN ? 0 : 1;

      if (biome == WATER || biome == LAKE || biome == MARSH || biome == RIVER)
        lakesMask[index] = 0;
      else
        lakesMask[index] = 1;
    }
  }

  // Normalizations
  for (size_t i = 0; i < size*size; i++) {
    elevationMask[i] /= maxHeight;
  }

  _elevationMask.setPixels(elevationMask);
  _islandMask.setPixels(islandMask);
  _lakesMask.setPixels(lakesMask);
}

void ReliefGenerator::generateRelief(size_t size) {
  convertMapData(size);
  _islandMask.smoothDilatation(50);
  _lakesMask.smoothDilatation(10);

  Perlin perlinRelief(3, 0.06, 0.75, size);
  GeneratedImage randomRelief(perlinRelief.getPixels());
  randomRelief.multiply(_islandMask.getPixels());
  randomRelief.multiply(_lakesMask.getPixels());

  _relief.setPixels(_elevationMask.getPixels());
  _relief.addAndNormalize(randomRelief.getPixels(), 0.5);
}
