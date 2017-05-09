#pragma once

#include "mapInfoExtractor.h"

class ReliefGenerator {
public:
  ReliefGenerator (const MapInfoExtractor& mapInfoExtractor);

  // Run all steps to have the final relief image
  void generateRelief();

  inline void saveToFile(std::string filename) const {_relief.saveToFile(filename);}
  inline const GeneratedImage& getRelief() const {return _relief;}

private:
  const MapInfoExtractor& _mapInfoExtractor;

  GeneratedImage _relief;
};
