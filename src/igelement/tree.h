#pragma once
#include "igElement.h"
#include "treeTexManager.h"
#include "utils.h"

class Tree : public igElement {
public:
	Tree(sf::Vector2f position, TreeTexManager* _manager, Biome _biome, int _index);

	void draw() const;

protected:
	TreeTexManager* _manager;

	Biome _biome;
	int _index;
};
