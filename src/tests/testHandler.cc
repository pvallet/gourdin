#ifndef NDEBUG

#include "testHandler.hpp"

#include <SDL_image.h>
#include <SDL2pp/SDL2pp.hh>
#include <cstdio>
#include <fstream>
#include <sstream>

#include "generatedImage.h"
#include "reliefGenerator.h"

#define DELETE_LIST_NAME "to_delete"

TestHandler::TestHandler (const Clock& beginningOfProg) :
  _beginningOfProg(beginningOfProg) {}

void TestHandler::saveToImage(std::vector<uint8_t> pixels, std::string filename) const {
  int size = sqrt(pixels.size() / 4);

  // Little endian
  SDL2pp::Surface img(&pixels[0], size, size, 32, 4*size, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
	IMG_SavePNG(img.Get(), filename.c_str());

  addToDeleteList(filename);
}

void TestHandler::addToDeleteList(std::string filename) const {
  std::ofstream deleteList;
  deleteList.open(DELETE_LIST_NAME, std::ios::app);
  deleteList << filename << "\n";
  deleteList.close();
}

void TestHandler::saveToImage(const std::vector<float>& pixels, std::string filename) const {
  std::vector<uint8_t> rgbPixels(4*pixels.size());

  for (int i = 0; i < pixels.size(); i++) {
    for (int j = 0; j < 3; j++) {
      rgbPixels[4*i + j] = pixels[i] * 255;
    }
    rgbPixels[4*i + 3] = 255;
  }

  saveToImage(rgbPixels, filename);
}

std::vector<float> TestHandler::generateTestSquare(size_t size) {
  std::vector<float> res(size*size, 0);

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (i > size/3 && i < 2*size/3 && j > size/3 && j < 2*size/3)
        res[i*size + j] = 0;
      else
        res[i*size + j] = 1;
    }
  }

  return res;
}

std::vector<float> TestHandler::generateTestCircle(size_t size) {
  std::vector<float> res(size*size, 0);

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      float centeredI = i - size / 2.f;
      float centeredJ = j - size / 2.f;

      if (sqrt(centeredI*centeredI + centeredJ*centeredJ) < size / 6)
        res[i*size + j] = 0;
      else
        res[i*size + j] = 1;
    }
  }

  return res;
}

void TestHandler::ContentGeneratorDisplayForestsMask(
  const ContentGenerator& contentGenerator, std::string savename) const {

  const std::vector<std::vector<bool> >& forestsMask = contentGenerator.getForestsMask();

  std::vector<uint8_t> pixels(forestsMask.size() * forestsMask.size() * 4, 255);

	for (int i = 0 ; i < forestsMask.size() ; i++) { // Convert mask to array of pixels
		for (int j = 0 ; j < forestsMask.size() ; j++) {
      if (forestsMask[i][j]) {
        pixels[i*4*forestsMask.size() + j*4] = 0;
        pixels[i*4*forestsMask.size() + j*4 + 2] = 0;
      }
		}
	}

  saveToImage(pixels, savename);
}

void TestHandler::displayEngineGeneratedComponents(const Engine& engine) const {
  ContentGeneratorDisplayForestsMask(engine._contentGenerator, "contents.png");

  saveToImage(engine._terrainGeometry.getReliefGenerator().getPixels(), "relief_relief.png");

  // If we generated the relief from the map and not loaded it from a previous generation
  if (engine._mapInfoExtractor.getSize() != 0) {
    saveToImage(engine._mapInfoExtractor.getLakesElevations().getPixels(), "relief_lakesElevations.png");
    saveToImage(engine._mapInfoExtractor.getElevationMask().getPixels(), "relief_elevationMask.png");

    // Test image fusion
    std::array<GeneratedImage, (int) Biome::BIOME_NB_ITEMS> plainImages;
    for (int i = 0; i < (int) Biome::BIOME_NB_ITEMS; i++) {
      plainImages[i] = GeneratedImage(512, i / (float) (int) Biome::BIOME_NB_ITEMS);
    }
    std::array<const GeneratedImage*, (int) Biome::BIOME_NB_ITEMS> toSend;
    for (int i = 0; i < (int) Biome::BIOME_NB_ITEMS; i++) {
      toSend[i] = &plainImages[i];
    }

    saveToImage(engine._mapInfoExtractor.imageFusion(toSend).getPixels(), "biomeTransitions.png");

    // Display additional reliefs
    for (auto it = engine._reliefGenerator._biomesAdditionalRelief.begin();
             it != engine._reliefGenerator._biomesAdditionalRelief.end(); it++) {

      std::stringstream convert;
      convert << "biome_" << (int) it->first << ".png";
      saveToImage(it->second.getPixels(), convert.str());
    }

    saveToImage(engine._reliefGenerator._additionalRelief.getPixels(), "biome_combined.png");
  }
}

void TestHandler::testVecUtils() const {
  glm::vec3 testVec(1,8,-2);
  glm::vec3 testVecSpherical = ut::spherical(testVec);
  glm::vec3 testVecBack = ut::carthesian(testVecSpherical);

  float precision = 0.00001;

  if (glm::length(testVec - testVecBack) < precision)
    std::cout << "OK     - Conversion to spherical coordinates and back with precision " << precision << '\n';
  else {
    std::cout << "FAILED - Conversion to spherical coordinates and back with precision " << precision << '\n';
    std::cout << "         vector (" << testVec.x << "," << testVec.y << "," << testVec.z << ") "
              << "has been converted to (" << testVecSpherical.x << "," << testVecSpherical.y << "," << testVecSpherical.z << ") "
              << "and back to  (" << testVecBack.x << "," << testVecBack.y << "," << testVecBack.z << ") " << '\n';
  }
}

void TestHandler::testPerlin() const {
  Perlin perlin(3, 0.06, 0.1, 512);

  for (int i = 0; i < 1; i++) {
    std::ostringstream convert;
    convert << "perlin_" << i << ".png";
    perlin.shuffle();
    saveToImage(perlin.getPixels(), convert.str());
  }
}

void TestHandler::testGeneratedImage() const {
  // Test image
  GeneratedImage testCircle(generateTestCircle(512));
  saveToImage(testCircle.getPixels(), "test_circle.png");

  // Filters
  GeneratedImage boxFilter = testCircle;
  Clock boxFilterTime;
  boxFilter.applyConvolutionFilter(GeneratedImage::generateBoxFilter(20));
  std::cout << "Box filter time (20): " << boxFilterTime.getElapsedTime() << '\n';
  saveToImage(boxFilter.getPixels(), "test_circle_box_filter.png");

  GeneratedImage gaussianFilter = testCircle;
  Clock gaussianFilterTime;
  gaussianFilter.applyConvolutionFilter(GeneratedImage::generateGaussianFilter(20, 1/5.f));
  std::cout << "Gaussian filter time (20): " << gaussianFilterTime.getElapsedTime() << '\n';
  saveToImage(gaussianFilter.getPixels(), "test_circle_gaussian_filter.png");

  GeneratedImage smoothDilatation = testCircle;
  Clock smoothDilatationTime;
  smoothDilatation.smoothBlackDilatation(20);
  std::cout << "Smooth dilatation time (20): " << smoothDilatationTime.getElapsedTime() << '\n';
  saveToImage(smoothDilatation.getPixels(), "test_circle_smooth_dilatation.png");

  GeneratedImage dilatation = testCircle;
  Clock dilatationTime;
  dilatation.dilatation(20, [](float pixel) {return pixel != 1;});
  std::cout << "Dilatation time (20): " << dilatationTime.getElapsedTime() << '\n';
  saveToImage(dilatation.getPixels(), "test_circle_dilatation.png");

  // Operators
  GeneratedImage operations = testCircle;
  operations *= 0.5;
  saveToImage(operations.getPixels(), "test_circle_multiply.png");
  operations += 0.5;
  saveToImage(operations.getPixels(), "test_circle_multiply-add.png");
  operations -= 0.5;
  saveToImage(operations.getPixels(), "test_circle_multiply-add-substract.png");
  operations /= 0.5;
  saveToImage(operations.getPixels(), "test_circle_multiply-add-substract-divide.png");

  // Combine
  GeneratedImage white(512, 1.f);
  Perlin randomAdding(3, 0.06, 0.1, 512);
  white.combine(randomAdding.getPixels(), smoothDilatation.getPixels());
  saveToImage(white.getPixels(), "test_circle_smooth_dilatation_combine_perlin.png");

  // Invert
  GeneratedImage invert = testCircle;
  invert.invert();
  saveToImage(invert.getPixels(), "invert.png");

  // File I/O
  Perlin iotest(3, 0.06, 0.1, 512);
  GeneratedImage imgSaver(iotest.getPixels());
  imgSaver.saveToFile("testSave.png");
  GeneratedImage imgLoader;
  imgLoader.loadFromFile("testSave.png");

  if (imgSaver.getPixels() == imgLoader.getPixels())
    std::cout << "OK     - Save/Loading of a generated image" << '\n';
  else {
    std::cout << "FAILED - Save/Loading of a generated image" << '\n';
  }

  addToDeleteList("testSave.png");
}

void TestHandler::testAngleFunctions() const {
  std::pair<float,float> solutions = ut::solveAcosXplusBsinXequalC(3, sqrt(3), -sqrt(6));

  float precision = 1e-4;

  if (std::abs(solutions.first - 11*M_PI/12.f/RAD) < precision && std::abs(solutions.second - 17*M_PI/12.f/RAD) < precision)
    std::cout << "OK     - Solving a*cos(x) + b*sin(x) = c with precision " << precision << '\n';

  else {
    std::cout << "FAILED - Solving a*cos(x) + b*sin(x) = c" << '\n';
    std::cout << "         Solutions are (" << solutions.first << "," << solutions.second << "), "
              << "should be (" << 11*M_PI/12.f/RAD << "," << 17*M_PI/12.f/RAD << ")." << '\n';
  }

  float angle1 = 350;
  float angle2 = 20;

  if (ut::absDistBetweenAngles(angle1, angle2) == 30 &&
      ut::absDistBetweenAngles(angle2, angle1) == 30)
    std::cout << "OK     - Absolute distance between two angles mod 360" << '\n';

  else {
    std::cout << "FAILED - Absolute distance between two angles mod 360" << '\n';
    std::cout << "         Distance is " << ut::absDistBetweenAngles(angle1,angle2)
              << ", should be " << 30 << '\n';
  }

  angle1 = 350;
  angle2 = 20;
  float angle3 = 180;
  float angle4 = 10;

  if (ut::angleIsPositive(angle1, angle2) &&
     !ut::angleIsPositive(angle2, angle1) &&
     !ut::angleIsPositive(angle3, angle4) &&
      ut::angleIsPositive(angle4, angle3))
    std::cout << "OK     - Check if difference of two angle is positive or negative" << '\n';
  else
    std::cout << "FAILED - Check if difference of two angle is positive or negative" << '\n';

  angle1 = 952;
  angle2 = -750;

  if (ut::clampAngle(angle1) != 232)
    std::cout << "FAILED - Clamp angles: returned " << ut::clampAngle(angle1) << " for 952 instead of 232" << '\n';
  else if (ut::clampAngle(angle2) != 330)
    std::cout << "FAILED - Clamp angles: returned " << ut::clampAngle(angle2) << " for -750 instead of 330" << '\n';
  else
    std::cout << "OK     - Clamp angles" << '\n';
}

void TestHandler::runTests(const Controller& controller) const {
  std::cout << "Initialization time: " << _beginningOfProg.getElapsedTime() << '\n';
  displayEngineGeneratedComponents(controller._engine);
  testVecUtils();
  testPerlin();
  testGeneratedImage();
  testAngleFunctions();
}

void TestHandler::clean() const {
  std::string line;
  std::ifstream deleteList(DELETE_LIST_NAME);

  if (deleteList.is_open()) {
    while (getline(deleteList, line)){
      remove(line.c_str());
    }
    deleteList.close();
  }

  remove(DELETE_LIST_NAME);
}

#endif