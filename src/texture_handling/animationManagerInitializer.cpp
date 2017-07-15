#include "animationManagerInitializer.h"

#include <SDL_log.h>
#include <sstream>

#include "tinyxml.h"
#include "utils.h"

AnimationManagerInitializer::AnimationManagerInitializer () {
  _parameters.size = 0.f;
  _parameters.speed = 0.f;
}

void AnimationManagerInitializer::load(std::string folderPath) {
  std::string xmlFile = folderPath + "animInfo.xml";

  TiXmlDocument doc;
	doc.Parse(ut::textFileToString(xmlFile).c_str());

  TiXmlHandle hdl(&doc);
  TiXmlElement *elem = hdl.FirstChildElement("standardParams").Element();

  if (elem) {
    elem->QueryFloatAttribute("size", &_parameters.size);
    elem->QueryFloatAttribute("speed", &_parameters.speed);
  }

  elem = hdl.FirstChildElement("lstAnim").FirstChildElement().Element();

  if (elem == nullptr)
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "XML file: %s lacks animation information.", xmlFile.c_str());

  _maxHeight = 0;

  for (int i = 0; elem; i++) {
    AnimInfo anm;
    elem->QueryIntAttribute("steps", &anm.steps);
    elem->QueryIntAttribute("nbOri", &anm.orientations);

    int type;
    elem->QueryIntAttribute("duration", &anm.msDuration);
    elem->QueryIntAttribute("pause", &anm.msPause);
    elem->QueryIntAttribute("type", &type);

    if (anm.msPause == -1) {
      anm.loop = false;
      anm.msPause = 0;
    }

    else
      anm.loop = true;

    anm.texLayer = i;

    _animInfo.insert(std::pair<ANM_TYPE, AnimInfo>((ANM_TYPE) type, anm));
    elem = elem->NextSiblingElement();
  }

  _texArray.loadTextures(_animInfo.size(), folderPath);

  int i = 0;
  for (auto it = _animInfo.begin(); it != _animInfo.end(); it++) {
    glm::vec4 texRect = _texArray.getTexRectangle(i);
    it->second.spriteRect = glm::vec4(0,
                                      0,
                                      1.f / it->second.steps        * texRect.z,
                                      1.f / it->second.orientations * texRect.w);

    std::ostringstream convert;
    convert << folderPath << i << ".png";
    glm::vec2 texSize = _texArray.texSizes[i];
    it->second.spriteAbsoluteSize = glm::vec2(texSize.x / it->second.steps,
                                              texSize.y / it->second.orientations);

    if (it->second.spriteAbsoluteSize.y > _maxHeight)
      _maxHeight = it->second.spriteAbsoluteSize.y;

    i++;
  }
}
