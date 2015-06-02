#include "animationManager.h"
#include <tinyxml.h>
#include <iostream>

AnimationManager::AnimationManager(std::vector<sf::Texture*> _tex, std::string xml) :
	currentAnim(WAIT),
    currentSprite(0),
    alreadyElapsed(sf::Time::Zero),
    dead(false) {

    TiXmlDocument doc(xml);
    if(!doc.LoadFile()) {
        std::cerr << "Error while loading file: " << xml << std::endl;
        std::cerr << "Error #" << doc.ErrorId() << ": " << doc.ErrorDesc() << std::endl;
    }

    TiXmlHandle hdl(&doc);
    TiXmlElement *elem = hdl.FirstChildElement().FirstChildElement().Element();

    maxHeight = 0;

    for (unsigned int i = 0 ; i < _tex.size() ; i++) {
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

        tex.push_back(_tex[i]);

        anm.sprite = sf::IntRect(0, 0, tex[i]->getSize().x / anm.steps, tex[i]->getSize().y / anm.orientations);

        if (anm.sprite.height > maxHeight)
            maxHeight = anm.sprite.height;
        
        animInfo.insert(std::pair<ANM_TYPE, AnimInfo>((ANM_TYPE) type, anm));
        elem = elem->NextSiblingElement();
    }
}

void AnimationManager::launchAnimation(ANM_TYPE type) {
    if (!dead) {
        if (currentAnim != type) {
            currentAnim = type;

            currentSprite = 0;
        }
    }
}

void AnimationManager::update(sf::Time elapsed, float nOrientation) {
    alreadyElapsed += elapsed;

    if (currentSprite == animInfo[currentAnim].steps - 1 && animInfo[currentAnim].pause != sf::Time::Zero) {
        if (animInfo[currentAnim].loop) {
            if (alreadyElapsed.asMilliseconds() / animInfo[currentAnim].pause.asMilliseconds() != 0) {
                currentSprite = 0;
                alreadyElapsed = sf::milliseconds(alreadyElapsed.asMilliseconds() - animInfo[currentAnim].pause.asMilliseconds());
            }
        }
    }

    else {
        if (alreadyElapsed.asMilliseconds() / animInfo[currentAnim].duration.asMilliseconds() != 0) {
            currentSprite++;
            currentSprite %= animInfo[currentAnim].steps;
            alreadyElapsed = sf::milliseconds(alreadyElapsed.asMilliseconds() - animInfo[currentAnim].duration.asMilliseconds());
        }
    }

    currentOrient = getClosestOrient(nOrientation);
}

sf::IntRect AnimationManager::getCurrentSprite() {
	sf::IntRect sprite = animInfo[currentAnim].sprite;

    sprite.left += currentSprite * sprite.width;
    sprite.top += currentOrient * sprite.height;

    return sprite;
}

sf::Time AnimationManager::getAnimationTime(ANM_TYPE type) {
    return sf::milliseconds((animInfo[type].steps-1) * animInfo[type].duration.asMilliseconds());
}

int AnimationManager::getClosestOrient(float orientation) {
    /*if (tmp < 0.)
        tmp += 360.;

    if (tmp > 180.)
        tmp = 360. - tmp;*/

    float oriStep = 360. / (float) animInfo[currentAnim].orientations;

    return (animInfo[currentAnim].orientations - (int) round(orientation / oriStep)) % 8;
}

