#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "texArray.h"
#include "utils.h"

struct Flora {
	Biome biome;
	int nbTrees;
	int density; // Proximity of trees

	TextureArray texArray;
};

class TreeTexManager {
public:
	TreeTexManager();

	void load(std::string path);

	inline sf::Vector2f getSize(Biome biome, int index) const {
		return _flora[biome].texArray.texSizes[index] * _heightFactor;
	}
	inline sf::FloatRect getTexRectangle(Biome biome, int index) const {
		return _flora[biome].texArray.getTexRectangle(index);
	}
	inline void bind(Biome biome) const {
		glBindTexture(GL_TEXTURE_2D_ARRAY, _flora[biome].texArray.texID);
	}

	inline GLuint getTexID(Biome biome) const {return _flora[biome].texArray.texID;}

	inline float getDensity(Biome biome) const {return _flora[biome].density;}
	inline float getNBTrees(Biome biome) const {return _flora[biome].nbTrees;}

private:
	std::vector<Flora> _flora;

	float _heightFactor;
};
