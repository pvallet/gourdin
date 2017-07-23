#pragma once

#include <map>
#include <string>

#include "texArray.h"

enum ANM_TYPE {WAIT, WALK, DIE, RUN, ATTACK};

struct AnimInfo {
	int steps;
	int orientations;
	int msDuration; // of each frame
	int msPause; 	  // Before starting the anim over
	bool loop; 			// If false, pause is infinite

	glm::vec2 spriteAbsoluteSize;
	// Relative to the size of the texture
	// Two first coordinates are the coordinates of the top left corner, and two last the width and height
	glm::vec4 spriteRect;

	size_t texLayer;
};

struct AnimalParameters {
	float size;
	float speed;
};

/** One instance that will load the textures and metadata used commonly by
  * AnimationManager instances.
  */
class AnimationManagerInitializer {
public:
  AnimationManagerInitializer ();

  void load(std::string folderPath);

  inline void bind() const {
		glBindTexture(GL_TEXTURE_2D_ARRAY, _texArray.texID);
	}
	inline GLuint getTexID() const {return _texArray.texID;}

  inline int getMaxHeight() const {return _maxHeight;}
  inline const std::map<ANM_TYPE, AnimInfo>& getAnimInfo() const {return _animInfo;}
	inline const AnimalParameters& getParameters() const {return _parameters;}

private:
  int _maxHeight;

  std::map<ANM_TYPE, AnimInfo>  _animInfo;

	AnimalParameters _parameters;

	TextureArray _texArray;
};
