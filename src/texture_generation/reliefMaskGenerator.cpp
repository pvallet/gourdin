#include "reliefMaskGenerator.h"

#include <cmath>
#include <iostream>

ReliefMaskGenerator::ReliefMaskGenerator (const TerrainGeometry& terrainGeometry) :
  _terrainGeometry(terrainGeometry) {}

void ReliefMaskGenerator::generateMask(size_t size) {
  _maskSize = size;
  _islandMask.resize(size*size);

  for (size_t i = 0; i < size; i++) {
    for (size_t j = 0; j < size; j++) {
      _islandMask[i*size + j] = _terrainGeometry.getBiome(sf::Vector2f(
        i * MAX_COORD / size, j * MAX_COORD / size), 0) == OCEAN ? 0 : 1;
    }
  }
}

void ReliefMaskGenerator::smoothDilatation(float radius) {
  int dilSize = round(radius)+1;

  std::vector<float> dilMask(dilSize*dilSize);

  // Generate dilatation mask according to the distances. Only one quarter of it as it is symmetrical
  for (size_t i = 0; i < dilSize; i++) {
    for (size_t j = 0; j < dilSize; j++) {
      dilMask[i*dilSize + j] = sqrt(i*i + j*j) / radius;
    }
  }

  for (int i = 0; i < _maskSize; i++) {
  for (int j = 0; j < _maskSize; j++) {
    // Mask is only applied to expand black regions
    if (_islandMask[i*_maskSize + j] == 0.f) {
      for (int k = std::max(0, i-dilSize+1); k < std::min((int)_maskSize, i+dilSize); k++) {
      for (int l = std::max(0, j-dilSize+1); l < std::min((int)_maskSize, j+dilSize); l++) {
        size_t currentIndex = k*_maskSize + l;
        if (_islandMask[currentIndex] == 0)
          continue;
        _islandMask[currentIndex] = std::min(_islandMask[currentIndex], dilMask[abs(k-i)*dilSize + abs(l-j)]);
      }
      }
    }
  }
  }
}
