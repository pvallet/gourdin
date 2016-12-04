#pragma once

#include <SFML/System.hpp>

#include <utils.h>
#include <vecUtils.h>

#include <array>
#include <list>
#include <tuple>
#include <unordered_map>
#include <vector>

#define SUBDIV_LVL 8

struct Triangle {
  std::array<sf::Vector3f,3> points;
  Biome biome;
  sf::Vector3f normal;
};

struct Point {
  sf::Vector3f pos;
  sf::Vector3f normal;
  float distToBiomeEdge;
  std::list<Triangle*> belongsToTris;
};

struct hashFunc{
  size_t operator()(const sf::Vector3f &k) const{
  size_t h1 = std::hash<float>()(k.x);
  size_t h2 = std::hash<float>()(k.y);
  size_t h3 = std::hash<float>()(k.z);
  return (h1 ^ (h2 << 1)) ^ h3;
  }
};

class TerrainGeometry {
public:
  TerrainGeometry () {}

  inline void goingToAddNPoints(size_t n) {_points.reserve(n);}
  void addTriangle(std::array<sf::Vector3f,3> p, Biome biome);

  void computeNormals();

  std::list<Triangle*> getTrianglesNearPos(sf::Vector2f pos);

private:
  std::array<sf::Vector2u, 2> getSubChunkInfo(sf::Vector2f pos);

  std::unordered_map<sf::Vector3f, Point, hashFunc> _points;
  std::list<Triangle> _triangles;

  // The triangles are sorted on a two level grid:
  // - The first level corresponds to the chunk containing the triangle (x_chunk*NB_CHUNKS + y_chunk)
  // - The second level corresponds to the subchunk (x_subchunk * SUBDIV_LVL + y_subchunk)
  // Each triangle can belong to several subchunks
  std::vector<std::vector<std::list<Triangle*> > > _trianglesInSubChunk;

};
