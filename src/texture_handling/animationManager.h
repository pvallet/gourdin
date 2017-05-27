#pragma once

#include <SFML/System.hpp>

#include <map>
#include <string>
#include <vector>

#include "animationManagerInitializer.h"

class AnimationManager {
public:
	AnimationManager(const AnimationManagerInitializer& init);

	size_t launchAnimation(ANM_TYPE type); // returns the layer index
	void update(sf::Time elapsed, float nOrientation);

	inline void bindTexture() const {_texManager.bind();}
	inline GLuint getTexID() const {return _texManager.getTexID();}

	sf::FloatRect getCurrentSprite() const;
	inline float getMaxHeightFactor() const {
		return (float) _texManager.getMaxHeight() / (float) _animInfo.at(_currentAnim).spriteSize.y;
	}

	inline sf::Vector2f getRawSize() const {return _animInfo.at(_currentAnim).spriteSize;}
	inline const AnimalParameters& getParameters() const {return _texManager.getParameters();}

	sf::Time getAnimationTime(ANM_TYPE type) const;

private:
	size_t getClosestOrient(float orientation) const;
	inline size_t getNextSprite(size_t currentSprite, sf::Time elapsed) const {
		return currentSprite + elapsed.asMilliseconds() / _animInfo.at(_currentAnim).duration.asMilliseconds();}

	ANM_TYPE _currentAnim;
	size_t _currentOrient;
	size_t _currentSprite;

	sf::Time _alreadyElapsed;
	std::map<ANM_TYPE, AnimInfo> _animInfo;

	const AnimationManagerInitializer& _texManager;
};
