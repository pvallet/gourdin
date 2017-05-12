#include "reliefGenerator.h"

#include <cmath>
#include <iostream>

ReliefGenerator::ReliefGenerator(const MapInfoExtractor& mapInfoExtractor) :
  _mapInfoExtractor(mapInfoExtractor) {}

void ReliefGenerator::fillAdditionalReliefs() {
  size_t size = _mapInfoExtractor.getSize();

  _addNoRelief = GeneratedImage(size, 0);

  // Dunes
  Perlin perlinDunes(1, 0.5, 0, size);
  GeneratedImage dunes(perlinDunes.getPixels());
  dunes -= 0.5;
  dunes *= 0.08;
  dunes += 0.05;
  _biomesAdditionalRelief[SUBTROPICAL_DESERT] = dunes;

  // Mountain summits
  Perlin perlinSummits(2, 0.1, 0.4, size);
  GeneratedImage summits(perlinSummits.getPixels());
  summits *= 0.8;
  summits -= 0.4;
  GeneratedImage summitsMask = _mapInfoExtractor.getBiomeMask(BARE) + _mapInfoExtractor.getBiomeMask(SCORCHED);
  summitsMask.smoothBlackDilatation(_mapInfoExtractor.getTransitionSize());
  summits *= summitsMask;

  _biomesAdditionalRelief[BARE] = summits;
  _biomesAdditionalRelief[SCORCHED] = summits;

  std::array<const GeneratedImage*, BIOME_NB_ITEMS> biomeFusion;

  for (size_t i = 0; i < BIOME_NB_ITEMS; i++) {
    Biome biome = (Biome) i;
    if (_biomesAdditionalRelief.find(biome) == _biomesAdditionalRelief.end())
      biomeFusion[i] = &_addNoRelief;
    else {
      _biomesAdditionalRelief.at(biome) *= 0.3;
      biomeFusion[i] = &_biomesAdditionalRelief.at(biome);
    }
  }

  _additionalRelief = _mapInfoExtractor.imageFusion(biomeFusion);
}

void ReliefGenerator::generateRelief() {
  GeneratedImage islandMask = _mapInfoExtractor.getBiomeMask(OCEAN);
  GeneratedImage lakesMask = _mapInfoExtractor.getBiomeMask(WATER) +
                             _mapInfoExtractor.getBiomeMask(LAKE) +
                             _mapInfoExtractor.getBiomeMask(MARSH) +
                             _mapInfoExtractor.getBiomeMask(RIVER);

  GeneratedImage lakesElevations = _mapInfoExtractor.getLakesElevations();
  GeneratedImage elevationMask = _mapInfoExtractor.getElevationMask();

  islandMask.invert();
  // islandMask.smoothBlackDilatation(10);

  // Lakes are now black
  lakesMask.invert();
  lakesMask.smoothBlackDilatation(5);
  lakesElevations.dilatation(5, [](float pixel) {return pixel != 1;});

  // Lakes are white for the combination with the lake elevation
  lakesMask.invert();
  elevationMask.combine(lakesElevations.getPixels(), lakesMask.getPixels());
  lakesMask.invert();

  fillAdditionalReliefs();

  _relief.setPixels(elevationMask.getPixels());

  _additionalRelief *= islandMask * lakesMask;

  _relief += _additionalRelief;
  _relief.normalize();
}
