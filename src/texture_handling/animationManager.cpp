#include "animationManager.h"

#include <cmath>
#include <iostream>

AnimationManager::AnimationManager(const AnimationManagerInitializer& init) :
	_currentAnim(WAIT),
  _currentSprite(0),
  _alreadyElapsed(sf::Time::Zero),
	_texManager(init) {

	_animInfo = _texManager.getAnimInfo();
}

size_t AnimationManager::launchAnimation(ANM_TYPE type) {
  if (_currentAnim != type) {
    _currentAnim = type;

    _currentSprite = 0;
  }

	return _animInfo[type].texLayer;
}

void AnimationManager::update(sf::Time elapsed, float nOrientation) {
	AnimInfo curAnm = _animInfo.at(_currentAnim);
  _alreadyElapsed += elapsed;

	// We make sure that the elapsed time does not extend one loop
	sf::Time totalAnimDuration = getAnimationTime(_currentAnim);
	_alreadyElapsed = sf::milliseconds(_alreadyElapsed.asMilliseconds() % totalAnimDuration.asMilliseconds());

	size_t nextSprite = getNextSprite(_currentSprite, _alreadyElapsed);

	// Simple case, no restart to handle
	if (nextSprite < curAnm.steps) {
		_alreadyElapsed -= (float) (nextSprite - _currentSprite) * curAnm.duration;
		_currentSprite = nextSprite;
	}

	else {
		if (!curAnm.loop)
			_currentSprite = curAnm.steps-1;

		else {
			_alreadyElapsed -= (float) (curAnm.steps-1 - _currentSprite) * curAnm.duration;

			// The sprite is in the pause
			if (_alreadyElapsed < curAnm.pause)
				_currentSprite = curAnm.steps-1;

			// The sprite has started a new loop
			else {
				_alreadyElapsed -= curAnm.pause;
				nextSprite = getNextSprite(0, _alreadyElapsed);
				_alreadyElapsed -= (float) nextSprite * curAnm.duration;
				_currentSprite = nextSprite;
			}
		}
	}

  _currentOrient = getClosestOrient(nOrientation);
}

sf::FloatRect AnimationManager::getCurrentSprite() const {
	sf::FloatRect sprite = _animInfo.at(_currentAnim).sprite;

  sprite.left += _currentSprite * sprite.width;
  sprite.top += _currentOrient * sprite.height;

  return sprite;
}

sf::Time AnimationManager::getAnimationTime(ANM_TYPE type) const {
  return (float) _animInfo.at(type).steps * _animInfo.at(type).duration
			         + _animInfo.at(type).pause;
}

size_t AnimationManager::getClosestOrient(float orientation) const {

  float oriStep = 360.f / (float) _animInfo.at(_currentAnim).orientations;

  return (_animInfo.at(_currentAnim).orientations - (size_t) std::round(orientation / oriStep))
	      % _animInfo.at(_currentAnim).orientations;
}
