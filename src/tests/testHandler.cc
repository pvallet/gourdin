#include "testHandler.hpp"

#include <SFML/Graphics.hpp>

#include <cstdio>
#include <fstream>

#include "generatedImage.h"
#include "reliefGenerator.h"
#include "vecUtils.h"

#define DELETE_LIST_NAME "to_delete"

TestHandler::TestHandler (const sf::Clock& beginningOfProg) :
  _beginningOfProg(beginningOfProg) {}

void TestHandler::saveToImage(const std::vector<sf::Uint8>& pixels, std::string filename) const {
  sf::Texture texture;
  int size = sqrt(pixels.size() / 4);
	texture.create(size, size);
	texture.update(&pixels[0]);

	texture.copyToImage().saveToFile(filename);

  std::ofstream deleteList;
  deleteList.open(DELETE_LIST_NAME, std::ios::app);
  deleteList << filename << "\n";
  deleteList.close();
}

void TestHandler::saveToImage(const std::vector<float>& pixels, std::string filename) const {
  std::vector<sf::Uint8> rgbPixels(4*pixels.size());

  for (size_t i = 0; i < pixels.size(); i++) {
    for (size_t j = 0; j < 3; j++) {
      rgbPixels[4*i + j] = pixels[i] * 255;
    }
    rgbPixels[4*i + 3] = 255;
  }

  saveToImage(rgbPixels, filename);
}

std::vector<float> TestHandler::generateTestSquare(size_t size) {
  std::vector<float> res(size*size, 0);

  for (size_t i = 0; i < size; i++) {
    for (size_t j = 0; j < size; j++) {
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

  for (size_t i = 0; i < size; i++) {
    for (size_t j = 0; j < size; j++) {
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

  std::vector<sf::Uint8> pixels(forestsMask.size() * forestsMask.size() * 4, 255);

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

void TestHandler::displayGameGeneratedComponents(const Game& game) const {
  ContentGeneratorDisplayForestsMask(game._contentGenerator, "contents.png");

  saveToImage(game._terrainGeometry.getReliefGenerator().getPixels(), "relief_relief.png");

  // If we generated the relief from the map and not loaded it from a previous generation
  if (game._mapInfoExtractor.getSize() != 0) {
    saveToImage(game._mapInfoExtractor.getLakesElevations().getPixels(), "relief_lakesElevations.png");
    saveToImage(game._mapInfoExtractor.getElevationMask().getPixels(), "relief_elevationMask.png");

    // Test image fusion
    std::array<GeneratedImage, BIOME_NB_ITEMS> plainImages;
    for (size_t i = 0; i < BIOME_NB_ITEMS; i++) {
      plainImages[i] = GeneratedImage(512, i / (float) BIOME_NB_ITEMS);
    }
    std::array<const GeneratedImage*, BIOME_NB_ITEMS> toSend;
    for (size_t i = 0; i < BIOME_NB_ITEMS; i++) {
      toSend[i] = &plainImages[i];
    }

    saveToImage(game._mapInfoExtractor.imageFusion(toSend).getPixels(), "biomeTransitions.png");

    // Display additional reliefs
    for (auto it = game._reliefGenerator._biomesAdditionalRelief.begin();
             it != game._reliefGenerator._biomesAdditionalRelief.end(); it++) {

      std::stringstream convert;
      convert << "biome_" << it->first << ".png";
      saveToImage(it->second.getPixels(), convert.str());
    }

    saveToImage(game._reliefGenerator._additionalRelief.getPixels(), "biome_combined.png");
  }
}

void TestHandler::testVecUtils() const {
  sf::Vector3f testVec(1,8,-2);
  sf::Vector3f testVecSpherical = vu::spherical(testVec);
  sf::Vector3f testVecBack = vu::carthesian(testVecSpherical);

  float precision = 0.00001;

  if (vu::norm(testVec - testVecBack) < precision)
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

  for (size_t i = 0; i < 1; i++) {
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
  sf::Clock boxFilterTime;
  boxFilter.applyConvolutionFilter(GeneratedImage::generateBoxFilter(20));
  std::cout << "Box filter time (20): " << boxFilterTime.getElapsedTime().asMilliseconds() << '\n';
  saveToImage(boxFilter.getPixels(), "test_circle_box_filter.png");

  GeneratedImage gaussianFilter = testCircle;
  sf::Clock gaussianFilterTime;
  gaussianFilter.applyConvolutionFilter(GeneratedImage::generateGaussianFilter(20, 1/5.f));
  std::cout << "Gaussian filter time (20): " << gaussianFilterTime.getElapsedTime().asMilliseconds() << '\n';
  saveToImage(gaussianFilter.getPixels(), "test_circle_gaussian_filter.png");

  GeneratedImage smoothDilatation = testCircle;
  sf::Clock smoothDilatationTime;
  smoothDilatation.smoothBlackDilatation(20);
  std::cout << "Smooth dilatation time (20): " << smoothDilatationTime.getElapsedTime().asMilliseconds() << '\n';
  saveToImage(smoothDilatation.getPixels(), "test_circle_smooth_dilatation.png");

  GeneratedImage dilatation = testCircle;
  sf::Clock dilatationTime;
  dilatation.dilatation(20, [](float pixel) {return pixel != 1;});
  std::cout << "Dilatation time (20): " << dilatationTime.getElapsedTime().asMilliseconds() << '\n';
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
  else
    std::cout << "FAILED - Save/Loading of a generated image" << '\n';

  remove("testSave.png");
}

void TestHandler::testEventHandlerGame(const EventHandlerGame& eHandlerGame) const {
  std::pair<float,float> solutions = eHandlerGame.solveAcosXplusBsinXequalC(3, sqrt(3), -sqrt(6));

  if (solutions.first == 11*M_PI/12.f/RAD && solutions.second == 17*M_PI/12.f/RAD)
    std::cout << "OK     - Solving a*cos(x) + b*sin(x) = c" << '\n';

  else {
    std::cout << "FAILED - Solving a*cos(x) + b*sin(x) = c" << '\n';
    std::cout << "         Solutions are (" << solutions.first << "," << solutions.second << "), "
              << "should be (" << 11*M_PI/12.f/RAD << "," << 17*M_PI/12.f/RAD << ")." << '\n';
  }
}

void TestHandler::runTests(const Controller& controller) const {
  std::cout << "Initialization time: " << _beginningOfProg.getElapsedTime().asMilliseconds() << '\n';
  // displayGameGeneratedComponents(controller._game);
  testVecUtils();
  // testPerlin();
  // testGeneratedImage();
  testEventHandlerGame(controller._eHandlerGame);
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
