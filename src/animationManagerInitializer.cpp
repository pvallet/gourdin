#include "AnimationManagerInitializer.h"

#include <sstream>

#include <tinyxml.h>

void AnimationManagerInitializer::load(std::string folderPath) {
  std::string xmlFile = folderPath + "animInfo.xml";

  TiXmlDocument doc(xmlFile);

  if(!doc.LoadFile()) {
    std::cerr << "Error while loading file: " << xmlFile << std::endl;
    std::cerr << "Error #" << doc.ErrorId() << ": " << doc.ErrorDesc() << std::endl;
  }

  TiXmlHandle hdl(&doc);
  TiXmlElement *elem = hdl.FirstChildElement().FirstChildElement().Element();

  assert(elem && "Empty xml file");

  _maxHeight = 0;

  for (int i = 0; elem; i++) {
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

    anm.sprite = sf::FloatRect(0, 0, 1. / anm.steps, 1. / anm.orientations);

    std::ostringstream convert;
    convert << folderPath << i << ".png";
    sf::Vector2u texSize = loadTexture(convert.str());
    anm.spriteSize = sf::Vector2f(texSize.x / anm.steps, texSize.y / anm.orientations);

    if (anm.spriteSize.y > _maxHeight)
      _maxHeight = anm.spriteSize.y;

    _animInfo.insert(std::pair<ANM_TYPE, AnimInfo>((ANM_TYPE) type, anm));
    _texIndexInTexManager.insert(std::pair<ANM_TYPE, size_t>((ANM_TYPE) type, i));
    elem = elem->NextSiblingElement();
  }
}
