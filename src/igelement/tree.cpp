#include "tree.h"
#include "animationManager.h"

Tree::Tree(sf::Vector2<double> position, TreeTexManager* manager, Biome biome, int index) :
	igElement(position),
	_manager(manager),
	_biome(biome),
	_index(index) {

  _size = _manager->getSize(_biome, _index);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo);

  glBufferSubData(GL_ARRAY_BUFFER, (12*sizeof *_vertices), (8*sizeof *_coord2D), _coord2D);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Tree::draw() const {
	_manager->bind(_biome, _index);

	igElement::draw();

  glBindTexture(GL_TEXTURE_2D, 0);
}
