#pragma once

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define BUFFER_OFFSET(a) ((char*)NULL + (a))

#define CHUNK_SIZE 512.f
#define NB_CHUNKS 50
#define MAX_COORD (CHUNK_SIZE*NB_CHUNKS)

#define TEX_FACTOR 3.f // Number of times the texture is repeated per chunk
#define TERRAIN_TEX_TRANSITION_SIZE 20.f

#define NB_BIOMES 20

#define RANDOMF (rand() / (float) RAND_MAX)

#define NB_ANIMALS 2
enum Animals {ANTILOPE, LION};

enum Biome {OCEAN, WATER, LAKE, ICE, MARSH, BEACH, RIVER,
			SNOW, TUNDRA, BARE, SCORCHED,
			TAIGA, SHRUBLAND, TEMPERATE_DESERT,
			TEMPERATE_RAIN_FOREST, TEMPERATE_DECIDUOUS_FOREST, GRASSLAND,
			TROPICAL_RAIN_FOREST, TROPICAL_SEASONAL_FOREST, SUBTROPICAL_DESERT,
			NO_DEFINED_BIOME
		};

bool glCheckError(const char *file, int line);


// regex to replace gl calls: ([_a-zA-Z]* = )?(gl[^ :;>]*\([^;]*\));
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
	inline std::string getText() const {return _text.str();}

private:
	LogText() {}

	std::ostringstream _text;
};
