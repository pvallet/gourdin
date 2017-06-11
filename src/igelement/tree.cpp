#include "tree.h"
#include "animationManager.h"

Tree::Tree(glm::vec2 position, const TreeTexManager& manager, Biome biome, int index) :
	igElement(position),
	_manager(manager),
	_biome(biome),
	_index(index) {

  _size = _manager.getSize(_biome, _index);

	setVertices();
	setTexCoord(_manager.getTexRectangle(_biome, _index));
	setLayer(_index);
}
