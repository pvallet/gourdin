#include "skybox.h"

#define BUFFER_OFFSET(a) ((char*)NULL + (a))

Skybox::Skybox(std::string filename, Camera* camera) : 
	t(1.),
	pos{
		-t,-t,-t, -t, t,-t, -t,-t, t, -t, t, t, // XN
		t,-t,-t,  t,-t, t,  t, t,-t,  t, t, t  // XP
	},

	iXN { 0, 1, 2, 3},
	iXP { 4, 5, 6, 7},
	iYN { 0, 2, 4, 5},
	iYP { 1, 3, 6, 7},
	iZN { 0, 1, 4, 6},
	iZP { 2, 3, 5, 7},

	cam(camera) {

    // TEXTURES

	for (int i = 0 ; i < 6 ; i++) 
		textures.push_back(sf::Image());

	cube_map_target.push_back(GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB);
    cube_map_target.push_back(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB);
	cube_map_target.push_back(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB);
	cube_map_target.push_back(GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB);
    cube_map_target.push_back(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB);
    cube_map_target.push_back(GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB);

    textures[0].loadFromFile(filename + "XN.png");
    textures[1].loadFromFile(filename + "XP.png");
    textures[2].loadFromFile(filename + "YN.png");
    textures[3].loadFromFile(filename + "YP.png");
    textures[4].loadFromFile(filename + "ZN.png");
    textures[5].loadFromFile(filename + "ZP.png"); 

    glGenTextures(1, &cube_map_texture_ID);

    // VBO    

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(	GL_ARRAY_BUFFER, (48*sizeof *pos), NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, (24*sizeof *pos), pos); // Pos
    glBufferSubData(GL_ARRAY_BUFFER, (24*sizeof *pos), (24*sizeof *pos), pos); // TexCoord

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // IBO

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24 * sizeof *iXN, NULL, GL_STATIC_DRAW);    
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, 4 * sizeof *iXN, iXN);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof *iXP, 4 * sizeof *iXP, iXP);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 8 * sizeof *iYN, 4 * sizeof *iYN, iYN);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 12 * sizeof *iYP, 4 * sizeof *iYP, iYP);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 16 * sizeof *iZN, 4 * sizeof *iZN, iZN);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 20 * sizeof *iZP, 4 * sizeof *iZP, iZP);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Skybox::~Skybox() {
	glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
}

void Skybox::draw() const {
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, cube_map_texture_ID);

    sf::Image im;

    for (int i = 0; i < 6; i++) {
        im = textures[i];
        glTexImage2D(cube_map_target[i], 0, 3, im.getSize().x, im.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, im.getPixelsPtr());
    }

	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glRotatef( 90 - cam->getPhi(), 1.0f, 0.0f, 0.0f );
	glRotatef( cam->getTheta(), 0.0f, 1.0f, 0.0f );

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));
    glTexCoordPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(24*sizeof *pos));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(4*sizeof *iXN));
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(8*sizeof *iXN));
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(12*sizeof *iXN));
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(16*sizeof *iXN));
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(20*sizeof *iXN));

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, 0);

	glPopMatrix();
}

