#pragma once

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>

#include <string>
#include <vector>

#include "texManager.h"
#include "utils.h"

typedef struct Flora Flora;
struct Flora {
	Biome biome;
	int nbTrees;
	int density; // Proximity of trees
	int extension; // Size of forests
	std::vector<sf::Vector2u> size;
	std::vector<GLuint> tex;
};

class TreeTexManager : public TexManager {
public:
	TreeTexManager();

	void load(std::string path);

	inline float getHeight(Biome biome, int index) const {return _flora[biome].size[index].y * _heightFactor;}
	inline sf::Vector2u getSize(Biome biome, int index) const {return _flora[biome].size[index];}
	inline void bind(Biome biome, int index) {glBindTexture(GL_TEXTURE_2D, _flora[biome].tex[index]);}

	inline float getDensity(Biome biome) const {return _flora[biome].density;}
	inline float getExtension(Biome biome) const {return _flora[biome].extension;}
	inline float getNBTrees(Biome biome) const {return _flora[biome].nbTrees;}

private:
	std::vector<Flora> _flora;

	float _heightFactor;
};
