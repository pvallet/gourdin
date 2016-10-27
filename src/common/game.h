#pragma once

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include <map>
#include <vector>
#include <set>

#include "heightmap.h"
#include "ocean.h"

#include "lion.h"
#include "antilope.h"
#include "tree.h"

#include "animationManager.h"
#include "animationManagerInitializer.h"
#include "map.h"
#include "texManager.h"

#include "shader.h"
#include "utils.h"

struct compDepth {
  bool operator()(const igElement* a, const igElement* b) const
  {
    return a->getDepth() >= b->getDepth();
  }
};

class Game {
public:
	Game();

  void init();

	void generateNeighbourChunks(sf::Vector2i pos);
	void update(sf::Time elapsed);
	void render() const; // To be run on another thread
	void select(sf::IntRect rect, bool add);
	void moveSelection(sf::Vector2i screenTarget);
	void moveCamera(sf::Vector2f newAimedPos);
	void addLion(sf::Vector2i screenTarget);
	void generateHerd(sf::Vector2<double> pos, size_t count);

	inline const std::set<igElement*>& getSelection() const {return _sel;}
	inline const std::vector<igElement*>& getElements() const {return _e;}
  inline const std::map<sf::Vector2i, Chunk*, compChunkPos>& getGeneratedTerrain() const {return _terrain;}
  inline const std::set<sf::Vector2i, compChunkPos>& getBorderTerrain() const {return _terrainBorder;}

private:
	sf::Vector2i neighbour(sf::Vector2i pos, int index) const;
	void generateHeightmap(sf::Vector2i pos);
	sf::Vector2<double> get2DCoord(sf::Vector2i screenTarget) const;
	void generateForests(sf::Vector2i pos);

	std::set<igElement*> _sel; // Selection
	std::set<igElement*, compDepth> _vis; // Visible
	std::vector<igElement*> _e; // Elements

	Map _map;
  AnimationManagerInitializer _antilopeTexManager;
  AnimationManagerInitializer _lionTexManager;
	TexManager _terrainTexManager;
	TreeTexManager _treeTexManager;
	Shader _hmapShader;
  Shader _igEShader;

	std::map<sf::Vector2i, Chunk*, compChunkPos> _terrain;
	std::set<sf::Vector2i, compChunkPos> _terrainBorder;
};
