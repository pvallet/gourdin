#pragma once

#include <SFML/System.hpp>

#include <utils.h>
#include <vecUtils.h>

#include <array>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define MAX_SUBDIV_LVL 4

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

  // If the triangles are not sorted yet, sorts them in clockwise order
  void sortTriangles();
  const Triangle* getNextTri(const Triangle* tri) const;
  std::pair<sf::Vector3f,sf::Vector3f> getBorder() const;

  void addAdjacentTriangle(const Triangle* tri);
  inline const std::list<const Triangle*>& getAdjacentTriangles() const {return _adjacentTriangles;}

private:
  bool _sorted;
  std::list<const Triangle*> _adjacentTriangles;
};

struct vertHashFunc{
  size_t operator()(const sf::Vector3f &k) const {
  size_t h1 = std::hash<float>()(k.x);
  size_t h2 = std::hash<float>()(k.y);
  size_t h3 = std::hash<float>()(k.z);
  return (h1 ^ (h2 << 1)) ^ h3;
  }
};

struct triHashFunc{
  size_t operator()(const Triangle& t) const {
  size_t h1 = vertHashFunc()(t.vertices[0]->pos);
  size_t h2 = vertHashFunc()(t.vertices[1]->pos);
  size_t h3 = vertHashFunc()(t.vertices[2]->pos);
  return (h1 ^ (h2 << 1)) ^ h3;
  }
};

inline bool operator == (const Triangle& lhs, const Triangle& rhs) {
  return lhs.vertices == rhs.vertices;
}

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
    void subdivideTriangles(std::list<const Triangle*>& triangles);
    void computeNormals    (std::list<Vertex*>&   vertices);
    void computeNormals();

    bool isOcean(size_t x, size_t y) const;

    static std::list<Vertex*> getVertices(const std::list<const Triangle*> triangles);

    std::list<const Triangle*> getTrianglesInChunk(size_t x, size_t y) const;
    std::list<const Triangle*> getTrianglesNearPos  (sf::Vector2f pos) const;
    Biome getBiome(sf::Vector2f pos) const;



  private:
    static std::array<sf::Vector2u, 2> getSubChunkInfo(sf::Vector2f pos);

    std::unordered_map<sf::Vector3f, Vertex, vertHashFunc> _vertices;
    std::unordered_set<Triangle, triHashFunc> _triangles;

    // The triangles are sorted on a two level grid:
    // - The first level corresponds to the chunk containing the triangle (x_chunk*NB_CHUNKS + y_chunk)
    // - The second level corresponds to the subchunk (x_subchunk * GRID_SUBDIV + y_subchunk) (GRID_SUBDIV in cpp)
    // Each triangle can belong to several subchunks
    std::vector<std::vector<std::list<const Triangle*> > > _trianglesInSubChunk;

    friend class TerrainGeometry;
  };

  TerrainGeometry ();

  // For initialization
  inline SubdivisionLevel* getFirstSubdivLevel() {return _subdivisionLevels[0].get();}

  void generateNewSubdivisionLevel();

  inline bool isOcean(size_t x, size_t y) const {
    return _subdivisionLevels[0]->isOcean(x,y);}

  inline size_t getCurrentGlobalSubdivLvl() const {return _currentGlobalSubdivLvl;}
  std::list<const Triangle*> getTrianglesInChunk(size_t x, size_t y, size_t subdivLvl);
  std::list<const Triangle*> getTrianglesNearPos  (sf::Vector2f pos, size_t subdivLvl) const;
  Biome getBiome(sf::Vector2f pos, size_t subdivLvl) const;

private:
  void subdivideChunk(size_t x, size_t y, size_t subdivLvl);

  std::vector<std::unique_ptr<SubdivisionLevel> > _subdivisionLevels;

  // Deepest available subdivision level for every chunk (x_chunk*NB_CHUNKS + y_chunk)
  std::vector<size_t> _chunkSubdivLvl;

  size_t _currentGlobalSubdivLvl;
};
