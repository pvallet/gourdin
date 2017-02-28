#include "tree.h"
#include "animationManager.h"

Tree::Tree(sf::Vector2f position, const TreeTexManager& manager, Biome biome, int index) :
	igElement(position),
	_manager(manager),
	_biome(biome),
	_index(index),
	_heightSet(false) {

  _size = _manager.getSize(_biome, _index);

	setTexCoord(_manager.getTexRectangle(_biome, _index));
	setLayer(_index);
}
