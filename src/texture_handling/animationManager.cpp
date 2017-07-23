#include "animationManager.h"

#include <cmath>

AnimationManager::AnimationManager(const AnimationManagerInitializer& init) :
	_currentAnim(WAIT),
  _currentSprite(0),
  _msAlreadyElapsed(0),
	_texManager(init) {

	_animInfo = _texManager.getAnimInfo();
}

size_t AnimationManager::launchAnimation(ANM_TYPE type) {
  if (_currentAnim != type) {
    _currentAnim = type;
    _currentSprite = 0;
		_msAlreadyElapsed = 0;
  }

	return _animInfo[type].texLayer;
}

void AnimationManager::update(int msElapsed, float nOrientation) {
	AnimInfo curAnm = _animInfo.at(_currentAnim);
  _msAlreadyElapsed += msElapsed;

	// We make sure that the elapsed time does not extend one loop
	int msTotalAnimDuration = getAnimationTime(_currentAnim);
	_msAlreadyElapsed = _msAlreadyElapsed % msTotalAnimDuration;

	size_t nextSprite = getNextSprite(_currentSprite, _msAlreadyElapsed);

	// Simple case, no restart to handle
	if (nextSprite < curAnm.steps) {
		_msAlreadyElapsed -= (float) (nextSprite - _currentSprite) * curAnm.msDuration;
		_currentSprite = nextSprite;
	}

	else {
		if (!curAnm.loop)
			_currentSprite = curAnm.steps-1;

		else {
			_msAlreadyElapsed -= (float) (curAnm.steps-1 - _currentSprite) * curAnm.msDuration;

			// The sprite is in the pause
			if (_msAlreadyElapsed < curAnm.msPause)
				_currentSprite = curAnm.steps-1;

			// The sprite has started a new loop
			else {
				_msAlreadyElapsed -= curAnm.msPause;
				nextSprite = getNextSprite(0, _msAlreadyElapsed);
				_msAlreadyElapsed -= (float) nextSprite * curAnm.msDuration;
				_currentSprite = nextSprite;
			}
		}
	}

  _currentOrient = getClosestOrient(nOrientation);
}

glm::vec4 AnimationManager::getCurrentSpriteRect() const {
	glm::vec4 spriteRect = _animInfo.at(_currentAnim).spriteRect;

  spriteRect.x += _currentSprite * spriteRect.z;
  spriteRect.y += _currentOrient * spriteRect.w;

  return spriteRect;
}

int AnimationManager::getAnimationTime(ANM_TYPE type) const {
  return (float) _animInfo.at(type).steps * _animInfo.at(type).msDuration
			         + _animInfo.at(type).msPause;
}

size_t AnimationManager::getClosestOrient(float orientation) const {

  float oriStep = 360.f / (float) _animInfo.at(_currentAnim).orientations;

  return (_animInfo.at(_currentAnim).orientations - (size_t) round(orientation / oriStep))
	      % _animInfo.at(_currentAnim).orientations;
}
