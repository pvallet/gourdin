#include "reliefGenerator.h"

#include <cmath>
#include <iostream>

#include "perlin.h"

ReliefGenerator::ReliefGenerator(const MapInfoExtractor& mapInfoExtractor) :
  _mapInfoExtractor(mapInfoExtractor) {}

void ReliefGenerator::generateRelief() {
  GeneratedImage islandMask = _mapInfoExtractor.getIslandMask();
  GeneratedImage lakesMask = _mapInfoExtractor.getLakesMask();
  GeneratedImage lakesElevations = _mapInfoExtractor.getLakesElevations();
  GeneratedImage elevationMask = _mapInfoExtractor.getElevationMask();

  islandMask.smoothDilatation(50);

  lakesMask.smoothDilatation(5);
  lakesElevations.nonWhiteDilatation(5);

  lakesMask.invert();
  elevationMask.combine(lakesElevations.getPixels(), lakesMask.getPixels());
  lakesMask.invert();

  // islandMask.loadFromFile("islandMask.png");

  Perlin perlinRelief(3, 0.06, 0.75, _mapInfoExtractor.getSize());
  GeneratedImage randomRelief(perlinRelief.getPixels());
  randomRelief.multiply(islandMask.getPixels());
  randomRelief.multiply(lakesMask.getPixels());

  _relief.setPixels(elevationMask.getPixels());
  // _relief.addAndNormalize(randomRelief.getPixels(), 0.5);
}
