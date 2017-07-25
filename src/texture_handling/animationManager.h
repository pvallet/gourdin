#pragma once

#include <map>
#include <string>
#include <vector>

#include "animationManagerInitializer.h"

class AnimationManager {
public:
	AnimationManager(const AnimationManagerInitializer& init);

	size_t launchAnimation(ANM_TYPE type); // returns the layer index
	void update(int msElapsed, float nOrientation);

	inline void bindTexture() const {_texManager.bind();}
	inline const TextureArray* getTexArray() const {return _texManager.getTexArray();}

	glm::vec4 getCurrentSpriteRect() const;
	inline float getMaxHeightFactor() const {
		return (float) _texManager.getMaxHeight() / (float) _animInfo.at(_currentAnim).spriteAbsoluteSize.y;
	}

	inline glm::vec2 getRawSize() const {return _animInfo.at(_currentAnim).spriteAbsoluteSize;}
	inline const AnimalParameters& getParameters() const {return _texManager.getParameters();}

	int getAnimationTime(ANM_TYPE type) const;

private:
	size_t getClosestOrient(float orientation) const;
	inline size_t getNextSprite(size_t currentSprite, int msElapsed) const {
		return currentSprite + msElapsed / _animInfo.at(_currentAnim).msDuration;}

	ANM_TYPE _currentAnim;
	size_t _currentOrient;
	size_t _currentSprite;

	int _msAlreadyElapsed;
	std::map<ANM_TYPE, AnimInfo> _animInfo;

	const AnimationManagerInitializer& _texManager;
};
