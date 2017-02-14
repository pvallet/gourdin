#pragma once

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include <set>
#include <unordered_set>
#include <vector>

#include "lion.h"
#include "antilope.h"
#include "tree.h"
#include "igElementDisplay.h"

#include "chunk.h"
#include "contentGenerator.h"
#include "map.h"
#include "ocean.h"
#include "terrainGeometry.h"

#include "terrainTexManager.h"

#include "shader.h"
#include "utils.h"

enum ChunkStatus {NOT_GENERATED, EDGE, NOT_VISIBLE, VISIBLE};

class Game {
public:
	Game();
  virtual ~Game() {}

  void init();

	void update(sf::Time elapsed);
	void render() const; // Returns the number of triangles and number of elements
	void select(sf::IntRect rect, bool add);
	void moveSelection(sf::Vector2i screenTarget);
	void moveCamera(sf::Vector2f newAimedPos);
	void addLion(sf::Vector2i screenTarget);

	inline const std::set<Controllable*>& getSelection() const {return _selectedElmts;}
	inline const std::vector<std::unique_ptr<igElement> >& getElements() const {return _igElements;}

  inline const std::vector<std::vector<ChunkStatus> >& getChunkStatus() const {return _chunkStatus;}

private:
  // When the coordinates are (size_t x, size_t y), they are coordinates of the chunk
	sf::Vector2i neighbour      (size_t x, size_t y, size_t index) const;
  void generateNeighbourChunks(size_t x, size_t y);
	void generateChunk          (size_t x, size_t y);
  void appendNewElements(std::vector<igElement*> elems);
  void updateMovingElementsStates();


	sf::Vector2f get2DCoord(sf::Vector2i screenTarget) const;

  std::vector<std::unique_ptr<igElement> > _igElements; // Elements

  // Raw pointers because the ownership is in _igElements
	std::set<Controllable*> _selectedElmts; // Selection
	std::vector<igElement*> _visibleElmts; // Visible
  std::unordered_set<igMovingElement*> _igMovingElements;

	igElementDisplay _igElementDisplay;
  ContentGenerator _contentGenerator;

  Map _map;
  Ocean _ocean;
  TerrainGeometry _terrainGeometry;

	TerrainTexManager _terrainTexManager;
	Shader _terrainShader;
  Shader _igEShader;

	std::vector<std::vector<std::unique_ptr<Chunk> > > _terrain;

  std::vector<std::vector<ChunkStatus> > _chunkStatus;
};
