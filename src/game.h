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
#include "animationManagerInitializer.h"
#include "map.h"
#include "texManager.h"
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
	void generateHerd(sf::Vector2f pos, size_t count);

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

	Map* _map;
  AnimationManagerInitializer _antilopeTexManager;
  AnimationManagerInitializer _lionTexManager;
	TexManager _terrainTexManager;
	TreeTexManager _treeTexManager;
	Shader _hmapShader;
  Shader _igEShader;

	std::map<sf::Vector2i, Chunk*, compChunkPos> _terrain;
	std::set<sf::Vector2i, compChunkPos> _terrainBorder;
	Skybox* _skybox;
};
