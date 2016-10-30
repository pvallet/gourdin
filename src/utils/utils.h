#pragma once

#include <SFML/System.hpp>

#define BUFFER_OFFSET(a) ((char*)NULL + (a))

#define CHUNK_SIZE 512.
#define NB_CHUNKS 50
#define MAX_COORD (CHUNK_SIZE*NB_CHUNKS)

#define NB_BIOMES 20

#define RANDOMF (rand() / (float) RAND_MAX)

enum Biome {OCEAN, WATER, LAKE, ICE, MARSH, BEACH, RIVER,
			SNOW, TUNDRA, BARE, SCORCHED,
			TAIGA, SHRUBLAND, TEMPERATE_DESERT,
			TEMPERATE_RAIN_FOREST, TEMPERATE_DECIDUOUS_FOREST, GRASSLAND,
			TROPICAL_RAIN_FOREST, TROPICAL_SEASONAL_FOREST, SUBTROPICAL_DESERT,
			NO_DEFINED_BIOME
		};

void _check_gl_error(const char *file, int line);

#define _glCheckError() _check_gl_error(__FILE__,__LINE__)
