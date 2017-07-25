#pragma once
#include "igElement.h"
#include "treeTexManager.h"
#include "utils.h"

class Tree : public igElement {
public:
	Tree(glm::vec2 position, const TreeTexManager& _manager, Biome _biome, int _index);

	inline Biome getBiome() const {return _biome;}

	inline const TextureArray* getTexArray() const {return _manager.getTexArray(_biome);}

protected:
	const TreeTexManager& _manager;

	Biome _biome;
	int _index;
};
