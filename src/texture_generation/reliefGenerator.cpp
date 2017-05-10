#include "reliefGenerator.h"

#include <cmath>
#include <iostream>

ReliefGenerator::ReliefGenerator(const MapInfoExtractor& mapInfoExtractor) :
  _mapInfoExtractor(mapInfoExtractor) {}

void ReliefGenerator::fillAdditionalReliefs() {
  size_t size = _mapInfoExtractor.getSize();

  _addNoRelief = GeneratedImage::generatePlainCanvas(size, 0);

  Perlin perlinSandDunes(3, 0.06, 0.75, size);
  _addSandDunes = perlinSandDunes.getPixels();

  for (size_t i = 0; i < BIOME_NB_ITEMS; i++) {
    _biomesAdditionalRelief[i] = &_addNoRelief;
  }

  _biomesAdditionalRelief[SUBTROPICAL_DESERT] = &_addSandDunes;

  _additionalRelief = _mapInfoExtractor.imageFusion(_biomesAdditionalRelief);
}

void ReliefGenerator::generateRelief() {
  GeneratedImage islandMask = _mapInfoExtractor.getIslandMask();
  GeneratedImage lakesMask = _mapInfoExtractor.getLakesMask();
  GeneratedImage lakesElevations = _mapInfoExtractor.getLakesElevations();
  GeneratedImage elevationMask = _mapInfoExtractor.getElevationMask();

  islandMask.smoothDilatation(50);
  // islandMask.loadFromFile("islandMask.png");

  lakesMask.smoothDilatation(5);
  lakesElevations.nonWhiteDilatation(5);

  lakesMask.invert();
  elevationMask.combine(lakesElevations.getPixels(), lakesMask.getPixels());
  lakesMask.invert();

  fillAdditionalReliefs();

  _relief.setPixels(elevationMask.getPixels());
  _relief.addAndNormalize(_additionalRelief.getPixels(), 0.1);
}
