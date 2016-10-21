#pragma once

#include <SFML/Graphics.hpp>

#include <string>

#include "texManager.h"

enum ANM_TYPE {WAIT, WALK, DIE, RUN, ATTACK};

typedef struct AnimInfo AnimInfo;
struct AnimInfo {
	int steps;
	int orientations;
	sf::Time duration; 	// of each frame
	sf::Time pause; 	// Before starting the anim over
	bool loop; 			// If false, pause is infinite

	sf::Vector2f spriteSize;
	sf::FloatRect sprite; // Relative to the size of the texture
};


/** One instance that will load the textures and metadata used commonly by
  * AnimationManager instances.
  */
class AnimationManagerInitializer : public TexManager {
public:
  AnimationManagerInitializer () {}

  void load(std::string folderPath);

  inline void bindTexture(ANM_TYPE type) const {TexManager::bindTexture(_texIndexInTexManager.at(type));}

  inline int getMaxHeight() const {return _maxHeight;}
  inline std::map<ANM_TYPE, AnimInfo> const & getAnimInfo() const {return _animInfo;}

private:
  int _maxHeight;

  std::map<ANM_TYPE, AnimInfo>  _animInfo;
  std::map<ANM_TYPE, size_t>    _texIndexInTexManager;
};
