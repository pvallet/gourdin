#pragma once

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "utils.h"

typedef struct Flora Flora;
struct Flora {
	Biome biome;
	int nbTrees;
	int density; // Proximity of trees
	int extension; // Size of forests
	std::vector<sf::Texture*> tex;
};

class TreeTexManager {
public:
	TreeTexManager(std::string path);
	~TreeTexManager();

	inline float getHeight(Biome biome, int index) const {return _flora[biome]->tex[index]->getSize().y * _heightFactor;}
	inline sf::Vector2u getSize(Biome biome, int index) const {return _flora[biome]->tex[index]->getSize();}
	inline void bind(Biome biome, int index) {sf::Texture::bind(_flora[biome]->tex[index], sf::Texture::CoordinateType::Pixels);}

	inline float getDensity(Biome biome) const {return _flora[biome]->density;}
	inline float getExtension(Biome biome) const {return _flora[biome]->extension;}
	inline float getNBTrees(Biome biome) const {return _flora[biome]->nbTrees;}

private:
	std::vector<Flora*> _flora;

	float _heightFactor;
};
