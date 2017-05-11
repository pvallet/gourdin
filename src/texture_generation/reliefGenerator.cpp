#include "reliefGenerator.h"

#include <cmath>
#include <iostream>

ReliefGenerator::ReliefGenerator(const MapInfoExtractor& mapInfoExtractor) :
  _mapInfoExtractor(mapInfoExtractor) {}

void ReliefGenerator::fillAdditionalReliefs() {
  size_t size = _mapInfoExtractor.getSize();

  // Core relief
  Perlin coreRelief(3, 0.05, 0.7, size);
  _addNoRelief = GeneratedImage(coreRelief.getPixels());
  _addNoRelief.multiply(0.4);

  // Dunes
  Perlin perlinDunes(1, 0.1, 0, size);
  Perlin perlinSmallerDunes(1, 0.5, 0, size);
  GeneratedImage dunes(perlinDunes.getPixels());
  dunes.multiply(0.3);
  _biomesAdditionalRelief[BEACH] = dunes;
  dunes.addAndNormalize(perlinSmallerDunes.getPixels(), 0.1);
  _biomesAdditionalRelief[SUBTROPICAL_DESERT] = dunes;

  std::array<const GeneratedImage*, BIOME_NB_ITEMS> biomeFusion;

  for (size_t i = 0; i < BIOME_NB_ITEMS; i++) {
    if (_biomesAdditionalRelief.find((Biome) i) == _biomesAdditionalRelief.end())
      biomeFusion[i] = &_addNoRelief;
    else
      biomeFusion[i] = &_biomesAdditionalRelief.at((Biome) i);
  }

  _additionalRelief = _mapInfoExtractor.imageFusion(biomeFusion);
}

void ReliefGenerator::generateRelief() {
  GeneratedImage islandMask = _mapInfoExtractor.getIslandMask();
  GeneratedImage lakesMask = _mapInfoExtractor.getLakesMask();
  GeneratedImage lakesElevations = _mapInfoExtractor.getLakesElevations();
  GeneratedImage elevationMask = _mapInfoExtractor.getElevationMask();

  islandMask.smoothDilatation(10);

  lakesMask.smoothDilatation(5);
  lakesElevations.nonWhiteDilatation(5);

  lakesMask.invert();
  elevationMask.combine(lakesElevations.getPixels(), lakesMask.getPixels());
  lakesMask.invert();

  fillAdditionalReliefs();

  _relief.setPixels(elevationMask.getPixels());

  _additionalRelief.multiply(islandMask.getPixels());
  _additionalRelief.multiply(lakesMask.getPixels());
  _relief.addAndNormalize(_additionalRelief.getPixels(), 0.3);
}
