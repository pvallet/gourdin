#pragma once
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <map>
#include <vector>
#include <set>

#include "heightmap.h"
#include "igElement.h"
#include "igMovingElement.h"
#include "skybox.h"
#include "camera.h"

struct compChunkPos {
    bool operator()(const sf::Vector2i & a, const sf::Vector2i & b) const
    {
        return a.x != b.x ? a.x < b.x : a.y < b.y;
    }
};

class Map {
public:
	Map(Camera* camera);
	~Map();

	void update(sf::Time elapsed);
	void render() const; 
	void select(sf::IntRect rect, bool add);
	void moveSelection(sf::Vector2i screenTarget);

	inline std::set<igElement*> getSelection() const {return sel;}
	inline std::vector<igElement*> getElements() const {return e;}

private:
	std::set<igElement*> sel; // Selection
	std::vector<igElement*> e; // Elements
	Camera* cam;

	std::map<sf::Vector2i, Chunk*, compChunkPos> map;
	Skybox* skybox;
};

