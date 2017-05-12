#include "reliefGenerator.h"

#include <cmath>
#include <iostream>

ReliefGenerator::ReliefGenerator(const MapInfoExtractor& mapInfoExtractor) :
  _mapInfoExtractor(mapInfoExtractor) {}

void ReliefGenerator::fillAdditionalReliefs() {
  size_t size = _mapInfoExtractor.getSize();

  // No relief
  _biomesAdditionalRelief[SNOW] = GeneratedImage(size,0);
  _biomesAdditionalRelief[ICE] = GeneratedImage(size,0);
  _biomesAdditionalRelief[TROPICAL_RAIN_FOREST] = GeneratedImage(size,0);

  // Hills
  Perlin perlinHills(3, 0.04, 0.4, size);
  GeneratedImage hills(perlinHills.getPixels());
  #pragma omp parallel for
  for (size_t i = 0; i < size*size; i++) {
    if (hills[i] < 0.3)
      hills[i] = 0.3;
  }
  hills -= 0.35;
  hills *= 0.4;
  GeneratedImage hillsMask = _mapInfoExtractor.getBiomeMask(BARE) +
                             _mapInfoExtractor.getBiomeMask(SNOW) +
                             _mapInfoExtractor.getBiomeMask(TAIGA) +
                             _mapInfoExtractor.getBiomeMask(TUNDRA) +
                             _mapInfoExtractor.getBiomeMask(SHRUBLAND) +
                             _mapInfoExtractor.getBiomeMask(TEMPERATE_DESERT) +
                             _mapInfoExtractor.getBiomeMask(TEMPERATE_RAIN_FOREST) +
                             _mapInfoExtractor.getBiomeMask(TEMPERATE_DECIDUOUS_FOREST) +
                             _mapInfoExtractor.getBiomeMask(GRASSLAND) +
                             _mapInfoExtractor.getBiomeMask(TROPICAL_SEASONAL_FOREST);

  hillsMask.smoothBlackDilatation(_mapInfoExtractor.getTransitionSize());
  _defaultRelief = hills * hillsMask;

  // Mountain summits
  Perlin perlinSummits(2, 0.1, 0.5, size);
  GeneratedImage summits(perlinSummits.getPixels());
  summits *= 1.1;
  summits -= 0.2;
  summits += hills;
  GeneratedImage summitsMask = _mapInfoExtractor.getBiomeMask(BARE) + _mapInfoExtractor.getBiomeMask(SCORCHED);
  summitsMask.smoothBlackDilatation(_mapInfoExtractor.getTransitionSize());
  summits *= summitsMask;
  _biomesAdditionalRelief[BARE] = summits;
  _biomesAdditionalRelief[SCORCHED] = summits;

  // Dunes
  Perlin perlinDunes(1, 0.5, 0, size);
  GeneratedImage dunes(perlinDunes.getPixels());
  dunes -= 0.5;
  dunes *= 0.08;
  dunes += 0.05;
  _biomesAdditionalRelief[SUBTROPICAL_DESERT] = dunes;

  // Mix the different additional reliefs
  std::array<const GeneratedImage*, BIOME_NB_ITEMS> biomeFusion;
  for (size_t i = 0; i < BIOME_NB_ITEMS; i++) {
    Biome biome = (Biome) i;
    if (_biomesAdditionalRelief.find(biome) == _biomesAdditionalRelief.end())
      biomeFusion[i] = &_defaultRelief;
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
