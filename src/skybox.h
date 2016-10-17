#pragma once
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "camera.h"

class Skybox {
public:
	Skybox(std::string filename, Camera* camera);
	~Skybox();

	void draw() const;
private:
	float t; // size of the skybox
	float _pos[24];

	GLuint _iXN[4];
	GLuint _iXP[4];
	GLuint _iYN[4];
	GLuint _iYP[4];
	GLuint _iZN[4];
	GLuint _iZP[4];

	GLuint _vbo;
	GLuint _ibo;

	Camera* _cam;

	std::vector<GLenum> _cube_map_target;
	GLuint 							_cube_map_texture_ID;
	std::vector<sf::Image> _textures;
};
