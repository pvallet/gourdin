#pragma once

#include <SFML/System.hpp>

#include <utils.h>
#include <vecUtils.h>

#include <array>
#include <list>
#include <tuple>
#include <unordered_map>
#include <vector>

// Subdivision of the chunks to store the triangles
#define GRID_SUBDIV 8

struct Vertex;

class Triangle {
public:
  std::array<Vertex*,3> vertices;
  Biome biome;
  sf::Vector3f normal;

  // indice 0 corresponds to the point refPoint in tri, and the other indices are
  // the other vertices in clockwise order
  std::array<size_t,3> sortIndices(sf::Vector3f refPoint) const;
};

class Vertex {
public:
  Vertex(): _sorted(false) {}

  sf::Vector3f pos;
  sf::Vector3f normal;
  std::list<Triangle*> belongsToTris;

  // If the triangles are not sorted yet, sorts them in clockwise order
  Triangle* getNextTri(Triangle* tri);

private:
  bool _sorted;
};

struct vertHashFunc{
  size_t operator()(const sf::Vector3f &k) const {
  size_t h1 = std::hash<float>()(k.x);
  size_t h2 = std::hash<float>()(k.y);
  size_t h3 = std::hash<float>()(k.z);
  return (h1 ^ (h2 << 1)) ^ h3;
  }
};

// Handles the different levels of subdivision of the geometry of the terrain
class TerrainGeometry {
public:
  // Class to handle a single subdivision level
  class SubdivisionLevel {
  public:
    SubdivisionLevel ();

    // Init methods
    inline void goingToAddNPoints(size_t n) {_vertices.reserve(n);}
    void addTriangle(std::array<sf::Vector3f,3> p, Biome biome);
    void computeNormals();

    bool isOcean(size_t x, size_t y) const;

    std::list<Triangle*> getTrianglesInChunk(size_t x, size_t y) const;
    std::list<Triangle*> getTrianglesNearPos  (sf::Vector2f pos) const;
    Biome getBiome(sf::Vector2f pos) const;



  private:
    std::array<sf::Vector2u, 2> getSubChunkInfo(sf::Vector2f pos) const;

    std::unordered_map<sf::Vector3f, Vertex, vertHashFunc> _vertices;
    std::list<Triangle> _triangles;

    // The triangles are sorted on a two level grid:
    // - The first level corresponds to the chunk containing the triangle (x_chunk*NB_CHUNKS + y_chunk)
    // - The second level corresponds to the subchunk (x_subchunk * GRID_SUBDIV + y_subchunk)
    // Each triangle can belong to several subchunks
    std::vector<std::vector<std::list<Triangle*> > > _trianglesInSubChunk;

    friend class TerrainGeometry;
  };

  TerrainGeometry ();

  // For initialization
  inline SubdivisionLevel& getFirstSubdivLevel() {return _subdivisionLevels[0];}

  void generateNewSubdivisionLevel();

  inline bool isOcean(size_t x, size_t y) const {
    return _subdivisionLevels[0].isOcean(x,y);}

  inline size_t getNbSubdivLevels() const {return _subdivisionLevels.size();}
  std::list<Triangle*> getTrianglesInChunk(size_t x, size_t y, size_t subdivLvl) const;
  std::list<Triangle*> getTrianglesNearPos  (sf::Vector2f pos, size_t subdivLvl) const;
  Biome getBiome(sf::Vector2f pos, size_t subdivLvl) const;

private:
  std::vector<SubdivisionLevel> _subdivisionLevels;

};
