#pragma once
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <map>
#include <vector>
#include <set>

#include "heightmap.h"
#include "ocean.h"
#include "skybox.h"
#include "camera.h"

#include "igElement.h"
#include "lion.h"
#include "antilope.h"
#include "tree.h"

#include "animationManager.h"
#include "map.h"
#include "terrainTexManager.h"
#include "treeTexManager.h"
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
	Game(Camera* camera, Map* map);
	~Game();

  void init();

	void generateNeighbourChunks(sf::Vector2i pos);
	void update(sf::Time elapsed);
	void render() const; // To be run on another thread
	void select(sf::IntRect rect, bool add);
	void moveSelection(sf::Vector2i screenTarget);
	void moveCamera(sf::Vector2f newAimedPos);
	void addLion(sf::Vector2i screenTarget);
	void generateHerd(sf::Vector2f pos, int count);

	inline std::set<igElement*> getSelection() const {return _sel;}
	inline std::vector<igElement*> getElements() const {return _e;}

private:
	sf::Vector2i neighbour(sf::Vector2i pos, int index) const;
	void generateHeightmap(sf::Vector2i pos);
	sf::Vector2<double> get2DCoord(sf::Vector2i screenTarget) const;
	void generateForests(sf::Vector2i pos);

	std::set<igElement*> _sel; // Selection
	std::set<igElement*, compDepth> _vis; // Visible
	std::vector<igElement*> _e; // Elements
	Camera* _cam;

	std::vector<sf::Texture*> _lionTex;
	std::vector<sf::Texture*> _antilopeTex;
	Map* _map;
	TerrainTexManager _terrainTexManager;
	TreeTexManager _treeTexManager;
	Shader _hmapShader;

	std::map<sf::Vector2i, Chunk*, compChunkPos> _terrain;
	std::set<sf::Vector2i, compChunkPos> _terrainBorder;
	Skybox* _skybox;
};
