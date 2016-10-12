#include "animationManager.h"
#include <tinyxml.h>
#include <iostream>

AnimationManager::AnimationManager(std::vector<sf::Texture*> tex, std::string xml) :
	_currentAnim(WAIT),
  _currentSprite(0),
  _dead(false),
  _alreadyElapsed(sf::Time::Zero) {

  TiXmlDocument doc(xml);
  if(!doc.LoadFile()) {
    std::cerr << "Error while loading file: " << xml << std::endl;
    std::cerr << "Error #" << doc.ErrorId() << ": " << doc.ErrorDesc() << std::endl;
  }

  TiXmlHandle hdl(&doc);
  TiXmlElement *elem = hdl.FirstChildElement().FirstChildElement().Element();

  _maxHeight = 0;

  for (unsigned int i = 0 ; i < tex.size() ; i++) {
    if(!elem)
      std::cerr << "Lack of info in xml file" << std::endl;

    AnimInfo anm;
    elem->QueryIntAttribute("steps", &anm.steps);
    elem->QueryIntAttribute("nbOri", &anm.orientations);

    int durationms, pausems, type;
    elem->QueryIntAttribute("duration", &durationms);
    elem->QueryIntAttribute("pause", &pausems);
    elem->QueryIntAttribute("type", &type);

    anm.duration = sf::milliseconds(durationms);

    if (pausems == -1) {
      anm.loop = false;
      anm.pause = sf::milliseconds(1);
    }

    else {
      anm.loop = true;
      anm.pause = sf::milliseconds(pausems);
    }

    _tex.push_back(tex[i]);

    anm.sprite = sf::IntRect(0, 0, _tex[i]->getSize().x / anm.steps, _tex[i]->getSize().y / anm.orientations);

    if (anm.sprite.height > _maxHeight)
      _maxHeight = anm.sprite.height;

    _animInfo.insert(std::pair<ANM_TYPE, AnimInfo>((ANM_TYPE) type, anm));
    elem = elem->NextSiblingElement();
  }
}

void AnimationManager::launchAnimation(ANM_TYPE type) {
  if (!_dead) {
    if (_currentAnim != type) {
      _currentAnim = type;

      _currentSprite = 0;
    }
  }
}

void AnimationManager::update(sf::Time elapsed, float nOrientation) {
  _alreadyElapsed += elapsed;

  if (_currentSprite == _animInfo[_currentAnim].steps - 1 && _animInfo[_currentAnim].pause != sf::Time::Zero) {
    if (_animInfo[_currentAnim].loop) {
      if (_alreadyElapsed.asMilliseconds() / _animInfo[_currentAnim].pause.asMilliseconds() != 0) {
        _currentSprite = 0;
        _alreadyElapsed = sf::milliseconds(_alreadyElapsed.asMilliseconds() - _animInfo[_currentAnim].pause.asMilliseconds());
      }
    }
  }

  else {
    if (_alreadyElapsed.asMilliseconds() / _animInfo[_currentAnim].duration.asMilliseconds() != 0) {
      _currentSprite++;
      _currentSprite %= _animInfo[_currentAnim].steps;
      _alreadyElapsed = sf::milliseconds(_alreadyElapsed.asMilliseconds() - _animInfo[_currentAnim].duration.asMilliseconds());
    }
  }

  _currentOrient = getClosestOrient(nOrientation);
}

sf::IntRect AnimationManager::getCurrentSprite() const {
	sf::IntRect sprite = _animInfo.find(_currentAnim)->second.sprite;

  sprite.left += _currentSprite * sprite.width;
  sprite.top += _currentOrient * sprite.height;

  return sprite;
}

sf::Time AnimationManager::getAnimationTime(ANM_TYPE type) {
  return sf::milliseconds((_animInfo[type].steps-1) * _animInfo[type].duration.asMilliseconds());
}

int AnimationManager::getClosestOrient(float orientation) {
  /*if (tmp < 0.)
      tmp += 360.;

  if (tmp > 180.)
      tmp = 360. - tmp;*/

  float oriStep = 360. / (float) _animInfo[_currentAnim].orientations;

  return (_animInfo[_currentAnim].orientations - (int) round(orientation / oriStep)) % 8;
}
