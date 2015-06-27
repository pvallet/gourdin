#include "igMovingElement.h"
#include "vecUtils.h"
#include "utils.h"
#include <cmath>
#include <iostream>

igMovingElement::igMovingElement(sf::Vector2<double> position, AnimationManager _graphics) :
	speed(0.), 
	moving(false), 
	dead(false),
	igElement(position),
	graphics(_graphics) {
}

void igMovingElement::draw() const {
	const sf::Texture* texture = graphics.getTexture();

    sf::Texture::bind(texture, sf::Texture::CoordinateType::Pixels);

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

void igMovingElement::launchAnimation(ANM_TYPE type) {
	height /= graphics.getCurrentSprite().height;
	graphics.launchAnimation(type);
	height *= graphics.getCurrentSprite().height;
}

void igMovingElement::stop() {
	launchAnimation(WAIT);
	moving = false;
	speed = 0.f;
}

void igMovingElement::update(sf::Time elapsed, float theta) {
	graphics.update(elapsed, getOrientation());
	igElement::update(elapsed, theta);

	if (!dead) {
		if (direction.x != 0. || direction.y != 0.) {
			float ori = vu::angle(sf::Vector2<double>(1.0f,0.0f), direction);
			setOrientation(ori - camOrientation);
			
			pos.x += direction.x * speed * elapsed.asSeconds();
			pos.y += direction.y * speed * elapsed.asSeconds();
		}
	}

	sf::IntRect rect = graphics.getCurrentSprite();
	coord2D[0] = rect.left;
	coord2D[1] = rect.top;
	coord2D[2] = rect.left + rect.width;
	coord2D[3] = rect.top;
	coord2D[4] = rect.left + rect.width;
	coord2D[5] = rect.top + rect.height;
	coord2D[6] = rect.left;
	coord2D[7] = rect.top + rect.height;

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferSubData(GL_ARRAY_BUFFER, (12*sizeof *vertices), (8*sizeof *coord2D), coord2D);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void igMovingElement::die() {
	launchAnimation(DIE);
	speed = 0.;
	moving = false;
	dead = true;
	graphics.die();
}


