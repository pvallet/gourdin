#pragma once
#include <SFML/System.hpp>

#define BUFFER_OFFSET(a) ((char*)NULL + (a))
#define CHUNK_SIZE 256.
#define NB_BIOMES 20

enum Biome {OCEAN, WATER, LAKE, ICE, MARSH, BEACH, RIVER,
			SNOW, TUNDRA, BARE, SCORCHED,
			TAIGA, SHRUBLAND, TEMPERATE_DESERT,
			TEMPERATE_RAIN_FOREST, TEMPERATE_DECIDUOUS_FOREST, GRASSLAND,
			TROPICAL_RAIN_FOREST, TROPICAL_SEASONAL_FOREST, SUBTROPICAL_DESERT};


static double randomD() {
	return (double) rand() / (double) RAND_MAX;
}

static float randomF() {
	return (float) rand() / (float) RAND_MAX;
}

struct compChunkPos {
    bool operator()(const sf::Vector2i & a, const sf::Vector2i & b) const
    {
        return a.x != b.x ? a.x < b.x : a.y < b.y;
    }
};

