#pragma once

#include <SFML/Graphics.hpp>

#include "animationManagerInitializer.h"
#include "treeTexManager.h"

#include "perlin.h"
#include "utils.h"

class igElement;

class ContentGenerator {
public:
  ContentGenerator();

  void init();

  void saveToImage(std::string savename);

  // std::vector<igElement*> generateHerdsInChunk(size_t x, size_t y);
  std::vector<igElement*> genHerd(sf::Vector2f pos, size_t count);

  std::vector<igElement*> genLion(sf::Vector2f pos);

private:
  Perlin _perlinGenerator;

  AnimationManagerInitializer _antilopeTexManager;
  AnimationManagerInitializer _lionTexManager;
  TreeTexManager _treeTexManager;
};
