#include "skybox.h"

#define BUFFER_OFFSET(a) ((char*)NULL + (a))

Skybox::Skybox(std::string filename, Camera* camera) :
	t(1.),
	_pos{
		-t,-t,-t, -t, t,-t, -t,-t, t, -t, t, t, // XN
		t,-t,-t,  t,-t, t,  t, t,-t,  t, t, t  // XP
	},

	_iXN { 0, 1, 2, 3},
	_iXP { 4, 5, 6, 7},
	_iYN { 0, 2, 4, 5},
	_iYP { 1, 3, 6, 7},
	_iZN { 0, 1, 4, 6},
	_iZP { 2, 3, 5, 7},

	_cam(camera) {

  // TEXTURES

	for (size_t i = 0 ; i < 6 ; i++)
		_textures.push_back(sf::Image());

	_cube_map_target.push_back(GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB);
  _cube_map_target.push_back(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB);
	_cube_map_target.push_back(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB);
	_cube_map_target.push_back(GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB);
  _cube_map_target.push_back(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB);
  _cube_map_target.push_back(GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB);

  _textures[0].loadFromFile(filename + "XN.png");
  _textures[1].loadFromFile(filename + "XP.png");
  _textures[2].loadFromFile(filename + "YN.png");
  _textures[3].loadFromFile(filename + "YP.png");
  _textures[4].loadFromFile(filename + "ZN.png");
  _textures[5].loadFromFile(filename + "ZP.png");

  glGenTextures(1, &_cube_map_texture_ID);

  // VBO

  glGenBuffers(1, &_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);

  glBufferData(	GL_ARRAY_BUFFER, (48*sizeof *_pos), NULL, GL_STATIC_DRAW);

  glBufferSubData(GL_ARRAY_BUFFER, 0, (24*sizeof *_pos), _pos); // Pos
  glBufferSubData(GL_ARRAY_BUFFER, (24*sizeof *_pos), (24*sizeof *_pos), _pos); // TexCoord

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // IBO

  glGenBuffers(1, &_ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24 * sizeof *_iXN, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, 4 * sizeof *_iXN, _iXN);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof *_iXP, 4 * sizeof *_iXP, _iXP);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 8 * sizeof *_iYN, 4 * sizeof *_iYN, _iYN);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 12 * sizeof *_iYP, 4 * sizeof *_iYP, _iYP);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 16 * sizeof *_iZN, 4 * sizeof *_iZN, _iZN);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 20 * sizeof *_iZP, 4 * sizeof *_iZP, _iZP);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Skybox::~Skybox() {
	glDeleteBuffers(1, &_vbo);
  glDeleteBuffers(1, &_ibo);
}

void Skybox::draw() const {
  glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, _cube_map_texture_ID);

  sf::Image im;

  for (size_t i = 0; i < 6; i++) {
    im = _textures[i];
    glTexImage2D(_cube_map_target[i], 0, 3, im.getSize().x, im.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, im.getPixelsPtr());
  }

	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glRotatef( 90 - _cam->getPhi(), 1.0f, 0.0f, 0.0f );
	glRotatef( _cam->getTheta(), 0.0f, 1.0f, 0.0f );

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(24*sizeof *_pos));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(2);

  glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
  glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(4*sizeof *_iXN));
  glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(8*sizeof *_iXN));
  glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(12*sizeof *_iXN));
  glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(16*sizeof *_iXN));
  glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(20*sizeof *_iXN));

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, 0);

	glPopMatrix();
}
