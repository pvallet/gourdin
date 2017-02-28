#pragma once
#include "igElement.h"
#include "treeTexManager.h"
#include "utils.h"

class Tree : public igElement {
public:
	Tree(sf::Vector2f position, const TreeTexManager& _manager, Biome _biome, int _index);
	virtual ~Tree() {}

	virtual void setHeight(float height) {igElement::setHeight(height); _heightSet = true;}

	virtual bool needsToUpdateHeight() const {return !_heightSet;}

	inline Biome getBiome() const {return _biome;}

	inline size_t getTexID() const {return _manager.getTexID(_biome);}

protected:
	const TreeTexManager& _manager;

	Biome _biome;
	int _index;

	bool _heightSet;
};
