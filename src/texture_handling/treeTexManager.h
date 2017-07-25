#pragma once

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

	inline glm::vec2 getSize(Biome biome, int index) const {
		return _flora[biome].texArray.getTexSizes()[index] * _heightFactor;
	}
	inline glm::vec4 getTexRectangle(Biome biome, int index) const {
		return _flora[biome].texArray.getTexRectangle(index);
	}
	inline void bind(Biome biome) const {_flora[biome].texArray.bind();}

	inline const TextureArray* getTexArray(Biome biome) const {return &_flora[biome].texArray;}

	inline float getDensity(Biome biome) const {return _flora[biome].density;}
	inline float getNBTrees(Biome biome) const {return _flora[biome].nbTrees;}

private:
	std::vector<Flora> _flora;

	float _heightFactor;
};
