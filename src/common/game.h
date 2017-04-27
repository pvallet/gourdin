#pragma once

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include <set>
#include <unordered_set>
#include <vector>

#include "antilope.h"
#include "human.h"
#include "lion.h"
#include "tree.h"
#include "igElementDisplay.h"

#include "chunk.h"
#include "contentGenerator.h"
#include "map.h"
#include "ocean.h"
#include "skybox.h"
#include "terrainGeometry.h"

#include "terrainTexManager.h"

#include "perlin.h"
#include "shader.h"
#include "utils.h"

enum ChunkStatus {NOT_GENERATED, EDGE, NOT_VISIBLE, VISIBLE};

class TestHandler;

class Game {

	friend TestHandler;

public:
	Game();

	void resetCamera();
  void init();

	void update(sf::Time elapsed);
	void render() const;
	void renderLifeBars(sf::RenderWindow& window) const;
	void select(sf::IntRect rect, bool add);
	void moveSelection(sf::Vector2i screenTarget);
	void moveCamera(sf::Vector2f newAimedPos);
	// Returns the character on which the player has clicked if so, otherwise returns focusedCharacter
	Human* moveCharacter(sf::Vector2i screenTarget, Human* focusedCharacter);
	void addLion(sf::Vector2i screenTarget);
	void genTribe(sf::Vector2f pos);

	inline void switchWireframe() {_wireframe = !_wireframe;}

	inline const std::set<Controllable*>& getSelection() const {return _selectedElmts;}
  inline const std::vector<std::vector<ChunkStatus> >& getChunkStatus() const {return _chunkStatus;}
	inline const std::vector<Human*>& getTribe() const {return _tribe;}

private:
  // When the coordinates are (size_t x, size_t y), they are coordinates of the chunk
	sf::Vector2i neighbour      (size_t x, size_t y, size_t index) const;
  void generateNeighbourChunks(size_t x, size_t y);
	void generateChunk          (size_t x, size_t y);
  void appendNewElements(std::vector<igMovingElement*> elems);
  void updateMovingElementsStates();
	void compute2DCorners();

	sf::Vector2f get2DCoord(sf::Vector2i screenTarget) const;

	bool _wireframe;

  std::vector<std::unique_ptr<igMovingElement> > _igMovingElements; // Elements stored in game

  // Raw pointers because the ownership is in _igMovingElements
	// Static elements are stored in chunks
	std::set<Controllable*> _selectedElmts;
  std::unordered_set<igMovingElement*> _activeElements;

	std::vector<Human*> _tribe;

	igElementDisplay _igElementDisplay;
  ContentGenerator _contentGenerator;

  Map _map;
  Ocean _ocean;
	Skybox _skybox;
  TerrainGeometry _terrainGeometry;
	Perlin _reliefGenerator;

	TerrainTexManager _terrainTexManager;
	Shader _terrainShader;
  Shader _igEShader;
	Shader _skyboxShader;

	std::vector<std::vector<std::unique_ptr<Chunk> > > _terrain;

  std::vector<std::vector<ChunkStatus> > _chunkStatus;
};
