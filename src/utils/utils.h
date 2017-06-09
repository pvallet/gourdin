#pragma once

#include <SFML/System.hpp>

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

sf::Vector2u convertToChunkCoords(sf::Vector2f pos);

bool glCheckError(const char *file, int line);

// regex to replace gl calls: ([_a-zA-Z]* = )?(gl[^ :;>]*\([^;]*\));
// replace with _glCheck($2);
#define _glCheck(expr) {expr; glCheckError(__FILE__,__LINE__);}


#include <sstream>

/** Singleton class in which any function can store info to be displayed on the
  * screen
	*/
class LogText	{
public:
	static LogText& getInstance() {
    static LogText instance;
    return instance;
  }

	LogText(LogText const&)         = delete;
	void operator=(LogText const&)  = delete;

	inline void clear() {_text.str("");}
	inline void addLine(std::string newLine) { _text << newLine;}
	void addFPSandCamInfo(sf::Time elapsed);
	inline std::string getText() const {return _text.str();}


private:
	LogText() {}

	std::ostringstream _text;
};
