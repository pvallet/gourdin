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
	float pos[24];

	GLuint iXN[4];
	GLuint iXP[4];
	GLuint iYN[4];
	GLuint iYP[4];
	GLuint iZN[4];
	GLuint iZP[4];

	GLuint vbo;
	GLuint ibo;

	Camera* cam;

	std::vector<GLenum> cube_map_target;
	GLuint cube_map_texture_ID;
	std::vector<sf::Image> textures;
};
