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

  const TerrainGeometry::SubdivisionLevel* firstSubdiv = _terrainGeometry.getFirstSubdivLevel();
  TerrainGeometry::SubdivisionLevel smoother(NULL);

  std::list<const Triangle*> currentTriangles = firstSubdiv->getTriangles();
  smoother.goingToAddNPoints(currentTriangles.size() * 2);
  smoother.subdivideTriangles(currentTriangles);

  float maxHeight = 0;

  for (size_t i = 0; i < size; i++) {
    for (size_t j = 0; j < size; j++) {
      sf::Vector2f pos(i * MAX_COORD / size, j * MAX_COORD / size);
      size_t index = i*size + j;
      Biome biome = smoother.getBiome(pos);

      elevationMask[index] = pow(Triangle::getHeight(pos, smoother.getTrianglesNearPos(pos)), 2);
      if (elevationMask[index] > maxHeight)
        maxHeight = elevationMask[index];

      islandMask[index] = biome == OCEAN ? 0 : 1;

      if (biome == WATER || biome == LAKE || biome == MARSH || biome == RIVER)
        lakesMask[index] = 0;
      else
        lakesMask[index] = 1;
    }
  }

  std::vector<float> lakesElevations = computeLakesElevations(size, smoother, maxHeight);

  // Normalizations
  for (size_t i = 0; i < size*size; i++) {
    elevationMask[i] /= maxHeight;
    lakesElevations[i] /= maxHeight;
  }

  _lakesElevations.setPixels(lakesElevations);
  _elevationMask.setPixels(elevationMask);
  _islandMask.setPixels(islandMask);
  _lakesMask.setPixels(lakesMask);
}

std::vector<float> ReliefGenerator::computeLakesElevations(
  size_t size, const TerrainGeometry::SubdivisionLevel& smoother, float baseColor) {
  // Lakes positions
  std::vector<size_t> lakesInit(size, -1);
  std::vector<std::vector<size_t> > lakes(size, lakesInit);
  // Sum of heights
  std::vector<float> lakesTotalElevation;
  // For averaging
  std::vector<size_t> lakesNumberOfPixels;

  // Fill lake info
  for (size_t i = 0; i < size; i++) {
    for (size_t j = 0; j < size; j++) {
      sf::Vector2f pos(i * MAX_COORD / size, j * MAX_COORD / size);
      Biome biome = smoother.getBiome(pos);

      if (biome == WATER || biome == LAKE || biome == MARSH || biome == RIVER) {
        float pixelHeight = pow(Triangle::getHeight(pos, smoother.getTrianglesNearPos(pos)), 2);

        if (i > 0 && lakes[i-1][j] != -1) {
          lakesTotalElevation[lakes[i-1][j]] += pixelHeight;
          lakesNumberOfPixels[lakes[i-1][j]] += 1;
          lakes[i][j] = lakes[i-1][j];
        }
        else if (j > 0 && lakes[i][j-1] != -1) {
          lakesTotalElevation[lakes[i][j-1]] += pixelHeight;
          lakesNumberOfPixels[lakes[i][j-1]] += 1;
          lakes[i][j] = lakes[i][j-1];
        }
        else {
          size_t newLake = lakesTotalElevation.size();
          lakesTotalElevation.push_back(pixelHeight);
          lakesNumberOfPixels.push_back(1);
          lakes[i][j] = newLake;
        }
      }
    }
  }

  // Normalize heights
  for (size_t i = 0; i < lakesTotalElevation.size(); i++) {
    lakesTotalElevation[i] /= (float) lakesNumberOfPixels[i];
  }

  std::vector<float> lakesElevations(size*size);
  for (size_t i = 0; i < size; i++) {
    for (size_t j = 0; j < size; j++) {
      if (lakes[i][j] == -1)
        lakesElevations[i*size + j] = baseColor;
      else
        lakesElevations[i*size + j] = lakesTotalElevation[lakes[i][j]];
    }
  }

  return lakesElevations;
}

void ReliefGenerator::generateRelief(size_t size) {
  convertMapData(size);
  _islandMask.smoothDilatation(50);

  _lakesMask.smoothDilatation(5);
  _lakesElevations.nonWhiteDilatation(5);
  
  _lakesMask.invert();
  _elevationMask.combine(_lakesElevations.getPixels(), _lakesMask.getPixels());
  _lakesMask.invert();

  // _islandMask.loadFromFile("islandMask.png");

  Perlin perlinRelief(3, 0.06, 0.75, size);
  GeneratedImage randomRelief(perlinRelief.getPixels());
  randomRelief.multiply(_islandMask.getPixels());
  randomRelief.multiply(_lakesMask.getPixels());

  _relief.setPixels(_elevationMask.getPixels());
  // _relief.addAndNormalize(randomRelief.getPixels(), 0.5);
}
