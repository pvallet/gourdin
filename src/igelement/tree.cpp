#include "tree.h"
#include "animationManager.h"

Tree::Tree(sf::Vector2f position, const TreeTexManager& manager, Biome biome, int index) :
	igElement(position),
	_manager(manager),
	_biome(biome),
	_index(index) {

  _size = _manager.getSize(_biome, _index);

	for (size_t i = 0; i < 4; i++) {
		_layer[i] = _index;
	}

	setTexCoord(_manager.getTexRectangle(_biome, _index));

	glBindBuffer   (GL_ARRAY_BUFFER, _vbo);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(_vertices) + sizeof(_coord2D),
		sizeof(_layer), &_layer[0]);
	glBindBuffer   (GL_ARRAY_BUFFER, 0);
}

size_t Tree::draw() const {
	_manager.bind(_biome);

	igElement::draw();

 glBindTexture(GL_TEXTURE_2D, 0);

	return 1;
}
