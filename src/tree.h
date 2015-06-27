#pragma once
#include "igElement.h"
#include "treeTexManager.h"
#include "utils.h"

class Tree : public igElement {
public:
	Tree(sf::Vector2<double> position, TreeTexManager* _manager, Biome _biome, int _index);

	void draw() const;

protected:
	TreeTexManager* manager;

	Biome biome;
	int index;
};

