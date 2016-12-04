#pragma once

#define M_PI 3.14159265358979323846

#define BUFFER_OFFSET(a) ((char*)NULL + (a))

#define CHUNK_SIZE 512.f
#define NB_CHUNKS 50
#define MAX_COORD (CHUNK_SIZE*NB_CHUNKS)

#define TEX_FACTOR 3.f // Number of times the texture is repeated per chunk
#define TERRAIN_TEX_TRANSITION_SIZE 20.f

#define NB_BIOMES 20

#define RANDOMF (rand() / (float) RAND_MAX)

enum Biome {OCEAN, WATER, LAKE, ICE, MARSH, BEACH, RIVER,
			SNOW, TUNDRA, BARE, SCORCHED,
			TAIGA, SHRUBLAND, TEMPERATE_DESERT,
			TEMPERATE_RAIN_FOREST, TEMPERATE_DECIDUOUS_FOREST, GRASSLAND,
			TROPICAL_RAIN_FOREST, TROPICAL_SEASONAL_FOREST, SUBTROPICAL_DESERT,
			NO_DEFINED_BIOME
		};

bool Check_gl_error(const char *file, int line);

#define GL_CHECK_ERROR() Check_gl_error(__FILE__,__LINE__)
