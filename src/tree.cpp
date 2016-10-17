#include "tree.h"
#include "animationManager.h"

Tree::Tree(sf::Vector2<double> position, TreeTexManager* manager, Biome biome, int index) :
	igElement(position),
	_manager(manager),
	_biome(biome),
	_index(index) {

  _height = _manager->getHeight(_biome, _index);

  _coord2D[0] = 0;
  _coord2D[1] = 0;
  _coord2D[2] = _manager->getSize(_biome, _index).x-1;
  _coord2D[3] = 0;
  _coord2D[4] = _manager->getSize(_biome, _index).x-1;
  _coord2D[5] = _manager->getSize(_biome, _index).y-1;
  _coord2D[6] = 0;
  _coord2D[7] = _manager->getSize(_biome, _index).y-1;

  glBindBuffer(GL_ARRAY_BUFFER, _vbo);

  glBufferSubData(GL_ARRAY_BUFFER, (12*sizeof *_vertices), (8*sizeof *_coord2D), _coord2D);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Tree::draw() const {
	_manager->bind(_biome, _index);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glVertexAttribPointer(2, 2, GL_INT, GL_FALSE, 0, BUFFER_OFFSET(12*sizeof *_vertices));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(2);

  glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glBindTexture(GL_TEXTURE_2D, 0);
}
