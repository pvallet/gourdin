#include "igElement.h"
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <cmath>
#include <iostream>
#include <ctime>

#define BUFFER_OFFSET(a) ((char*)NULL + (a))

igElement::igElement(sf::Vector2f position, AnimationManager _graphics) :
	pos(position),
	height(3.),
	graphics(_graphics),
	camOrientation(0.) {

	orientation = (float) rand() / (float) RAND_MAX * 360.f;

	vertices = new float[12];
	coord2D = new int[8];
	indices = new GLuint[4];

	for (int i = 0 ; i < 12 ; i++)
		vertices[i] = 0.f;

	vertices[0] = 0.;
	vertices[1] = 1.;
	vertices[2] = 0.;
	vertices[3] = 1.;
	vertices[4] = 1.;
	vertices[5] = 0.;
	vertices[6] = 0.;
	vertices[7] = 0.;
	vertices[8] = 1.;
	vertices[9] = 1.;
	vertices[10] = 0.;
	vertices[11] = 1.;

	for (int i = 0 ; i < 8 ; i++)
		coord2D[i] = 0;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(	GL_ARRAY_BUFFER, (24*sizeof *vertices), NULL, GL_DYNAMIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, (12*sizeof *vertices), vertices);
    //glBufferSubData(GL_ARRAY_BUFFER, (12*sizeof *vertices), (8*sizeof *coord2D), coord2D);
    glBufferSubData(GL_ARRAY_BUFFER, (12*sizeof *vertices), (12*sizeof *vertices), vertices);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

	for (int i = 0 ; i < 4 ; i++) {
		indices[i] = i;
	}

	glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof *indices, NULL, GL_STATIC_DRAW);    
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, 4 * sizeof *indices, indices);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

igElement::~igElement() {
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
	delete[] vertices;
	delete[] coord2D;
	delete[] indices;
}

void igElement::draw() const {
	const sf::Texture* texture = getTexture();

    sf::Texture::bind(texture, sf::Texture::CoordinateType::Pixels);
    sf::IntRect rect = getCurrentSprite();
    glBegin( GL_QUADS );
        glTexCoord2d(rect.left, rect.top);
    	//glColor3f( vertices[0], vertices[1], vertices[2]);
        glVertex3f( vertices[0], vertices[1], vertices[2]);
        
        glTexCoord2d(rect.left + rect.width,rect.top); 
        //glColor3f( vertices[3], vertices[4], vertices[5]);
        glVertex3f( vertices[3], vertices[4], vertices[5]);
        
        glTexCoord2d(rect.left + rect.width,rect.top + rect.height);
        //glColor3f( vertices[6], vertices[7], vertices[8]);
        glVertex3f( vertices[6], vertices[7], vertices[8]);
        
        glTexCoord2d(rect.left, rect.top + rect.height);
        //glColor3f( vertices[9], vertices[10], vertices[11]);
        glVertex3f( vertices[9], vertices[10], vertices[11]);
    glEnd();

    /*for (int i = 0 ; i < 4 ; i++) {
		std::cout << vertices[3*i] << " "
				  << vertices[3*i+1] << " " 
				  << vertices[3*i+2] << std::endl;
	}*/

	/*glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));
    //glTexCoordPointer(2, GL_INT, 0, BUFFER_OFFSET(12*sizeof *vertices));
    glColorPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(12*sizeof *vertices));// + 8*sizeof *coord2D));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glDrawElements(GL_TRIANGLES, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    //glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);*/

    glBindTexture(GL_TEXTURE_2D, 0);

    glMatrixMode(GL_TEXTURE); // sf::Texture::bind modifies the texture matrix, we need to set it back to identity
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
}

void igElement::update(sf::Time elapsed, float theta) {
	graphics.update(elapsed, orientation);
	
	setOrientation(orientation + camOrientation - theta); // Orientation moves opposite to the camera

	camOrientation = theta;
}

void igElement::set3DCorners(sf::Vector3f nCorners[4]) {
	for (int i = 0 ; i < 4 ; i++) {
		vertices[3*i]   = nCorners[i].x;
		vertices[3*i+1] = nCorners[i].y;
		vertices[3*i+2] = nCorners[i].z;
	}

	sf::IntRect rect = getCurrentSprite();
	coord2D[0] = rect.left;
	coord2D[1] = rect.top;
	coord2D[2] = rect.left + rect.width;
	coord2D[3] = rect.top;
	coord2D[4] = rect.left + rect.width;
	coord2D[5] = rect.top + rect.height;
	coord2D[6] = rect.left;
	coord2D[7] = rect.top + rect.height;
}

void igElement::launchAnimation(ANM_TYPE type) {
	height /= graphics.getCurrentSprite().height;
	graphics.launchAnimation(type);
	height *= graphics.getCurrentSprite().height;
}

void igElement::setOrientation(float nOrientation) {
	orientation = nOrientation;

	if (orientation < 0.)
		orientation += 360. + 360 * (int) (-orientation / 360);
	else
		orientation -= 360. * (int) (orientation / 360);
}
