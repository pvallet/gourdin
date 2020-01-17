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
		return getFlora(biome).texArray.getTexSizes()[index] * _heightFactor;
	}
	inline glm::vec4 getTexRectangle(Biome biome, int index) const {
		return getFlora(biome).texArray.getTexRectangle(index);
	}
	inline void bind(Biome biome) const {getFlora(biome).texArray.bind();}

	inline const TextureArray* getTexArray(Biome biome) const {return &getFlora(biome).texArray;}

	inline float getDensity(Biome biome) const {return getFlora(biome).density;}
	inline float getNBTrees(Biome biome) const {return getFlora(biome).nbTrees;}

private:
	inline const Flora& getFlora(Biome biome) const {return _flora[(int) biome];}

	std::vector<Flora> _flora;

	float _heightFactor;
};
