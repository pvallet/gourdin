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

struct Vertex;

struct Triangle {
  std::array<Vertex*,3> vertices;
  Biome biome;
  std::array<Biome,3> neighbourBiomes;
  sf::Vector3f normal;
};

struct Vertex {
  sf::Vector3f pos;
  sf::Vector3f normal;
  float distToBiomeEdge;
  std::list<Triangle*> belongsToTris;
};

struct compTri {
  bool operator()(const Triangle* lhs, const Triangle* rhs) const {
  for (size_t i = 0; i < 3; i++) {
    if (lhs->vertices[i] < rhs->vertices[i])
      return true;
    else if (lhs->vertices[i] > rhs->vertices[i])
      return false;
  }

  return false;
  }
};

struct equalTri {
  bool operator()(const Triangle* lhs, const Triangle* rhs) {
  return ( lhs->vertices == rhs->vertices);
  }
};

struct vertHashFunc{
  size_t operator()(const sf::Vector3f &k) const {
  size_t h1 = std::hash<float>()(k.x);
  size_t h2 = std::hash<float>()(k.y);
  size_t h3 = std::hash<float>()(k.z);
  return (h1 ^ (h2 << 1)) ^ h3;
  }
};

class TerrainGeometry {
public:
  TerrainGeometry ();

  inline void goingToAddNPoints(size_t n) {_vertices.reserve(n);}
  // biomes[i] = biome of the neighbour triangle on the other side of the edge
  // starting with point p[i] clockwise
  // biomes[3] = biome of the triangle
  void addTriangle(std::array<sf::Vector3f,3> p, std::array<Biome,4> biomes);

  void computeNormals();
  void computeDistancesToBiomeEdges();

  bool isOcean(size_t x, size_t y) const;

  std::list<Triangle*> getTrianglesInChunk(size_t x, size_t y) const;
  std::list<Triangle*> getTrianglesNearPos(sf::Vector2f pos) const;
  Triangle* getTriangleContaining(sf::Vector2f pos) const;

private:
  // indice 0 corresponds to the point refPoint in tri, and the other indices are
  // the other vertices in clockwise order
  std::array<size_t,3> sortIndices(sf::Vector3f refPoint, const Triangle& tri) const;
  std::array<sf::Vector2u, 2> getSubChunkInfo(sf::Vector2f pos) const;

  std::unordered_map<sf::Vector3f, Vertex, vertHashFunc> _vertices;
  std::list<Triangle> _triangles;

  // The triangles are sorted on a two level grid:
  // - The first level corresponds to the chunk containing the triangle (x_chunk*NB_CHUNKS + y_chunk)
  // - The second level corresponds to the subchunk (x_subchunk * SUBDIV_LVL + y_subchunk)
  // Each triangle can belong to several subchunks
  std::vector<std::vector<std::list<Triangle*> > > _trianglesInSubChunk;

};
