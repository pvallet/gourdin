#pragma once

#include <cstdlib>
#include <string>
#include <glm/glm.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define RAD (M_PI / 180.f)

#define CHUNK_SIZE 512.f
#define NB_CHUNKS 50
#define MAX_COORD (CHUNK_SIZE*NB_CHUNKS)

#define TEX_FACTOR 6.f // Number of times the texture is repeated per chunk
#define TERRAIN_TEX_TRANSITION_SIZE 20.f

#define RANDOMF (rand() / (float) RAND_MAX)

#define BUFFER_OFFSET(a) ((char*)NULL + (a))

enum Animals {ANTILOPE, DEER, LION, WOLF, LEOPARD,
	AOE1_MAN, AOE2_MAN, WOMAN, ANIMALS_NB_ITEMS};

	enum Biome {OCEAN, WATER, LAKE, ICE, MARSH, BEACH, RIVER,
		SNOW, TUNDRA, BARE, SCORCHED,
		TAIGA, SHRUBLAND, TEMPERATE_DESERT,
		TEMPERATE_RAIN_FOREST, TEMPERATE_DECIDUOUS_FOREST, GRASSLAND,
		TROPICAL_RAIN_FOREST, TROPICAL_SEASONAL_FOREST, SUBTROPICAL_DESERT,
		BIOME_NB_ITEMS
	};

// ut = utils
namespace ut {
	glm::uvec2 convertToChunkCoords(glm::vec2 pos);

	// Rectange a contains vector b
	template<typename valType >
	bool contains (glm::tvec4< valType > const &a, glm::tvec2< valType > const &b) {
	  if ((a.x - b.x) * (a.x + a.z - b.x) < 0 &&
	      (a.y - b.y) * (a.y + a.w - b.y) < 0)
	    return true;

	  return false;
	}

	glm::vec3 carthesian(float r, float theta, float phi);
	glm::vec3 spherical(float x, float y, float z);

	inline glm::vec3 carthesian(glm::vec3 u) {return carthesian(u.x,u.y,u.z);}
	inline glm::vec3 spherical (glm::vec3 u) {return spherical (u.x,u.y,u.z);}

	std::string textFileToString(const std::string& path);
}

bool glCheckError(const char *file, int line);

// regex to replace gl calls: ([_a-zA-Z]* = )?(gl[^ :;>]*\([^;]*\));
// replace with _glCheck($2);
#define _glCheck(expr) {expr; glCheckError(__FILE__,__LINE__);}
