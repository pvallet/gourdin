#pragma once

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
	std::vector<sf::Vector2f> size;
	std::vector<size_t> texIndices;
};

class TreeTexManager : public TexManager {
public:
	TreeTexManager();

	void load(std::string path);

	inline sf::Vector2f getSize(Biome biome, int index) const {return _flora[biome].size[index] * _heightFactor;}
	inline void bind(Biome biome, int index) const {bindTexture(_flora[biome].texIndices[index]);}

	inline float getDensity(Biome biome) const {return _flora[biome].density;}
	inline float getExtension(Biome biome) const {return _flora[biome].extension;}
	inline float getNBTrees(Biome biome) const {return _flora[biome].nbTrees;}

private:
	std::vector<Flora> _flora;

	float _heightFactor;
};
