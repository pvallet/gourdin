#include "tree.h"
#include "animationManager.h"

Tree::Tree(sf::Vector2<double> position, TreeTexManager* _manager, Biome _biome, int _index) :
	igElement(position),
	manager(_manager),
	biome(_biome),
	index(_index) {

    height = manager->getHeight(biome, index);

    coord2D[0] = 0;
    coord2D[1] = 0;
    coord2D[2] = manager->getSize(biome, index).x-1;
    coord2D[3] = 0;
    coord2D[4] = manager->getSize(biome, index).x-1;
    coord2D[5] = manager->getSize(biome, index).y-1;
    coord2D[6] = 0;
    coord2D[7] = manager->getSize(biome, index).y-1;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferSubData(GL_ARRAY_BUFFER, (12*sizeof *vertices), (8*sizeof *coord2D), coord2D);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Tree::draw() const {
	manager->bind(biome, index);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));
    glTexCoordPointer(2, GL_INT, 0, BUFFER_OFFSET(12*sizeof *vertices));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    glMatrixMode(GL_TEXTURE); // sf::Texture::bind modifies the texture matrix, we need to set it back to identity
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
}