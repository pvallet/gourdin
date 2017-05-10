#pragma once

#include "mapInfoExtractor.h"
#include "perlin.h"

// TestHandler has access to all the components of ReliefGenerator to easily display them
#ifndef NDEBUG
  class TestHandler;
#endif

class ReliefGenerator {
public:
  ReliefGenerator (const MapInfoExtractor& mapInfoExtractor);

  // Run all steps to have the final relief image
  void generateRelief();

  inline void saveToFile(std::string filename) const {_relief.saveToFile(filename);}
  inline const GeneratedImage& getRelief() const {return _relief;}

#ifndef NDEBUG
  friend TestHandler;
#endif

private:
  void fillAdditionalReliefs();

  const MapInfoExtractor& _mapInfoExtractor;

  std::array<const GeneratedImage*, BIOME_NB_ITEMS> _biomesAdditionalRelief;

  GeneratedImage _addNoRelief;
  GeneratedImage _addSandDunes;

  GeneratedImage _additionalRelief;
  GeneratedImage _relief;
};
