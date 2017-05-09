#include "mapInfoExtractor.h"

MapInfoExtractor::MapInfoExtractor(const TerrainGeometry& terrainGeometry) :
  _terrainGeometry(terrainGeometry),
  _size(0) {}

void MapInfoExtractor::convertMapData(size_t size) {
  std::vector<float> elevationMask(size*size);
  std::vector<float> islandMask(size*size);
  std::vector<float> lakesMask(size*size);
  _biomes.resize(size*size, OCEAN);
  _size = size;

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
      _biomes[index] = smoother.getBiome(pos);

      elevationMask[index] = pow(Triangle::getHeight(pos, smoother.getTrianglesNearPos(pos)), 2);
      if (elevationMask[index] > maxHeight)
        maxHeight = elevationMask[index];

      islandMask[index] = _biomes[index] == OCEAN ? 0 : 1;

      if (_biomes[index] == WATER || _biomes[index] == LAKE || _biomes[index] == MARSH || _biomes[index] == RIVER)
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

std::vector<float> MapInfoExtractor::computeLakesElevations(
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

void MapInfoExtractor::generateBiomesTransitions(float transitionSize) {
  int dilSize = round(transitionSize)+1;

  _biomesMasks.resize(_size*_size);
  std::vector<float> dilMask(dilSize*dilSize);

  // Generate dilatation mask according to the distances. Only one quarter of it as it is symmetrical
  for (size_t i = 0; i < dilSize; i++) {
    for (size_t j = 0; j < dilSize; j++) {
      dilMask[i*dilSize + j] = std::max(0., 1 - sqrt(i*i + j*j) / transitionSize);
    }
  }

  for (int i = 0; i < _size; i++) {
  for (int j = 0; j < _size; j++) {
    Biome biome = _biomes[i*_size + j];

    for (int k = std::max(0, i-dilSize+1); k < std::min((int)_size, i+dilSize); k++) {
    for (int l = std::max(0, j-dilSize+1); l < std::min((int)_size, j+dilSize); l++) {
      size_t currentIndex = k*_size + l;

      // Initialize a new biome inserted in the map
      if (_biomesMasks[currentIndex].find(biome) == _biomesMasks[currentIndex].end())
        _biomesMasks[currentIndex][biome] = 0;

      _biomesMasks[currentIndex][biome] = std::max(_biomesMasks[currentIndex][biome], dilMask[abs(k-i)*dilSize + abs(l-j)]);
    }
    }
  }
  }
}

GeneratedImage MapInfoExtractor::imageFusion(const std::array<const GeneratedImage*, BIOME_NB_ITEMS>& imgs) const {
  std::vector<float> result(_size*_size, 0);

  for (size_t i = 0; i < _size; i++) {
  for (size_t j = 0; j < _size; j++) {
    size_t currentIndex = i*_size + j;
    float normalizationFactor = 0;

    for (auto biome = _biomesMasks[currentIndex].begin(); biome != _biomesMasks[currentIndex].end(); biome++) {
      result[currentIndex] += biome->second * (*imgs[biome->first])[currentIndex];
      normalizationFactor += biome->second;
    }

    result[currentIndex] /= normalizationFactor;
  }
  }

  return GeneratedImage(result);
}
