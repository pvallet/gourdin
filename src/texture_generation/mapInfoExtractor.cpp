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

  #pragma omp parallel for collapse(2)
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
  #pragma omp parallel for
  for (size_t i = 0; i < size*size; i++) {
    elevationMask[i] /= maxHeight;
    lakesElevations[i] /= maxHeight;
  }

  _lakesElevations.setPixels(lakesElevations);
  _elevationMask.setPixels(elevationMask);
  _islandMask.setPixels(islandMask);
  _lakesMask.setPixels(lakesMask);
}

size_t MapInfoExtractor::getLowestIndexAdjacentLake(const std::vector<std::set<size_t> >& adjacentLakes,
  std::set<size_t>& nodesSeen, size_t node) {

  assert(node < adjacentLakes.size());

  if (nodesSeen.find(node) == nodesSeen.end()) {
    size_t min = node;
    nodesSeen.insert(node);
    for (auto it = adjacentLakes[node].begin(); it != adjacentLakes[node].end(); it++) {
      size_t minBranch = getLowestIndexAdjacentLake(adjacentLakes, nodesSeen, *it);
      if (minBranch < min)
        min = minBranch;
    }
    return min;
  }
  else // Return biggest unsigned int
    return -1;
}

std::vector<float> MapInfoExtractor::computeLakesElevations(
  size_t size, const TerrainGeometry::SubdivisionLevel& smoother, float baseColor) {
  std::vector<size_t> lakesInit(size, -1);
  // Lakes positions
  std::vector<std::vector<size_t> > lakes(size, lakesInit);
  // Sum of heights
  std::vector<float> lakesTotalElevation;
  // For averaging
  std::vector<size_t> lakesNumberOfPixels;

  // Graph containing all the adjacent lakes with strictly inferior indices.
  std::vector<std::set<size_t> > adjacentLakes;

  // Fill lake info
  for (size_t i = 0; i < size; i++) {
    for (size_t j = 0; j < size; j++) {
      sf::Vector2f pos(i * MAX_COORD / size, j * MAX_COORD / size);
      Biome biome = smoother.getBiome(pos);

      if (biome == WATER || biome == LAKE || biome == MARSH || biome == RIVER) {
        float pixelHeight = pow(Triangle::getHeight(pos, smoother.getTrianglesNearPos(pos)), 2);

        if (i > 0 && lakes[i-1][j] != -1) {
          size_t abovePixel = lakes[i-1][j];
          lakesTotalElevation[abovePixel] += pixelHeight;
          lakesNumberOfPixels[abovePixel] += 1;
          lakes[i][j] = abovePixel;

          if (j > 0 && lakes[i][j-1] != -1 && lakes[i][j-1] != abovePixel) {
            size_t max = std::max(lakes[i][j-1], abovePixel);
            size_t min = std::min(lakes[i][j-1], abovePixel);
            adjacentLakes[max].insert(min);
            adjacentLakes[min].insert(max);
          }
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
          adjacentLakes.push_back(std::set<size_t>());
          lakes[i][j] = newLake;
        }
      }
    }
  }

  // index is the current lake, and it points to the lowest indexed adjacent lake
  std::vector<size_t> baseLakes(lakesTotalElevation.size(), -1);

  // Simplify lakes adjacency to point always to the lowest one
  // (e.g. if lake 2 is lake 1 and lake 3 is lake 2, put lake 3 is lake 1)
  for (int i = baseLakes.size()-1; i >= 0; i--) {
    if (baseLakes[i] == -1) {
      std::set<size_t> nodesSeen;
      size_t baseLakeValue = getLowestIndexAdjacentLake(adjacentLakes, nodesSeen, i);
      for (auto it = nodesSeen.begin(); it != nodesSeen.end(); it++) {
        baseLakes[*it] = baseLakeValue;
      }
    }
  }

  // Fix lakes stats according to adjacency
  for (size_t i = 0; i < size; i++) {
    for (size_t j = 0; j < size; j++) {
      if (lakes[i][j] != -1 && baseLakes[lakes[i][j]] != lakes[i][j]) {
        lakesTotalElevation[baseLakes[lakes[i][j]]] += lakesTotalElevation[lakes[i][j]];
        lakesNumberOfPixels[baseLakes[lakes[i][j]]] += lakesNumberOfPixels[lakes[i][j]];
        lakes[i][j] = baseLakes[lakes[i][j]];
      }
    }
  }

  // Normalize heights
  for (size_t i = 0; i < lakesTotalElevation.size(); i++) {
    lakesTotalElevation[i] /= (float) lakesNumberOfPixels[i];
  }

  std::vector<float> lakesElevations(size*size);
  #pragma omp parallel for collapse(2)
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
  #pragma omp parallel for collapse(2)
  for (size_t i = 0; i < dilSize; i++) {
    for (size_t j = 0; j < dilSize; j++) {
      dilMask[i*dilSize + j] = std::max(0., 1 - sqrt(i*i + j*j) / transitionSize);
    }
  }

  #pragma omp parallel for collapse(2)
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
