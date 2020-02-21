#include "reliefGenerator.h"

#include <cmath>

ReliefGenerator::ReliefGenerator(const MapInfoExtractor& mapInfoExtractor) :
  _mapInfoExtractor(mapInfoExtractor) {}

void ReliefGenerator::fillAdditionalReliefs() {
  size_t size = _mapInfoExtractor.getSize();

  // No relief
  _biomesAdditionalRelief[Biome::SNOW] = GeneratedImage(size,0);
  _biomesAdditionalRelief[Biome::ICE] = GeneratedImage(size,0);
  _biomesAdditionalRelief[Biome::TROPICAL_RAIN_FOREST] = GeneratedImage(size,0);

  // Hills
  Perlin perlinHills(3, 0.04, 0.4, size);
  GeneratedImage hills(perlinHills.getPixels());
  #pragma omp parallel for
  for (int i = 0; i < size*size; i++) {
    if (hills[i] < 0.3)
      hills[i] = 0.3;
  }
  hills -= 0.35;
  hills *= 0.3;
  GeneratedImage hillsMask = _mapInfoExtractor.getBiomeMask(Biome::BARE) +
                             _mapInfoExtractor.getBiomeMask(Biome::SNOW) +
                             _mapInfoExtractor.getBiomeMask(Biome::TAIGA) +
                             _mapInfoExtractor.getBiomeMask(Biome::TUNDRA) +
                             _mapInfoExtractor.getBiomeMask(Biome::SHRUBLAND) +
                             _mapInfoExtractor.getBiomeMask(Biome::TEMPERATE_DESERT) +
                             _mapInfoExtractor.getBiomeMask(Biome::TEMPERATE_RAIN_FOREST) +
                             _mapInfoExtractor.getBiomeMask(Biome::TEMPERATE_DECIDUOUS_FOREST) +
                             _mapInfoExtractor.getBiomeMask(Biome::GRASSLAND) +
                             _mapInfoExtractor.getBiomeMask(Biome::TROPICAL_SEASONAL_FOREST);

  hillsMask.smoothBlackDilatation(_mapInfoExtractor.getTransitionSize());
  _defaultRelief = hills * hillsMask;

  // Mountain summits
  Perlin perlinSummits(2, 0.1, 0.5, size);
  GeneratedImage summits(perlinSummits.getPixels());
  summits *= 1.1;
  summits -= 0.2;
  summits += hills;
  GeneratedImage summitsMask = _mapInfoExtractor.getBiomeMask(Biome::BARE) + _mapInfoExtractor.getBiomeMask(Biome::SCORCHED);
  summitsMask.smoothBlackDilatation(_mapInfoExtractor.getTransitionSize());
  summits *= summitsMask;
  _biomesAdditionalRelief[Biome::BARE] = summits;
  _biomesAdditionalRelief[Biome::SCORCHED] = summits;

  // Dunes
  Perlin perlinDunes(1, 0.5, 0, size);
  GeneratedImage dunes(perlinDunes.getPixels());
  dunes -= 0.5;
  dunes *= 0.08;
  dunes += 0.05;
  _biomesAdditionalRelief[Biome::SUBTROPICAL_DESERT] = dunes;

  // Mix the different additional reliefs
  std::array<const GeneratedImage*, (int) Biome::BIOME_NB_ITEMS> biomeFusion;
  for (int i = 0; i < (int) Biome::BIOME_NB_ITEMS; i++) {
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
  GeneratedImage islandMask = _mapInfoExtractor.getBiomeMask(Biome::OCEAN);
  GeneratedImage lakesMask = _mapInfoExtractor.getBiomeMask(Biome::WATER) +
                             _mapInfoExtractor.getBiomeMask(Biome::LAKE) +
                             _mapInfoExtractor.getBiomeMask(Biome::MARSH) +
                             _mapInfoExtractor.getBiomeMask(Biome::RIVER);

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
