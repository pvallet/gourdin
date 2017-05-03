#include "testHandler.hpp"

#include <SFML/Graphics.hpp>

#include <cstdio>
#include <fstream>

#include "generatedImage.h"
#include "reliefGenerator.h"

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

std::vector<float> TestHandler::generateTestSquare(size_t size) const {
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

std::vector<float> TestHandler::generateTestCircle(size_t size) const {
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

  saveToImage(game._terrainGeometry.getReliefGenerator().getPixels(), "relief.png");
}

void TestHandler::testImageHandling() const {
  // Perlin
  Perlin perlin(3, 0.06, 0.1, 512);

  for (size_t i = 0; i < 1; i++) {
    std::ostringstream convert;
    convert << "perlin_" << i << ".png";
    perlin.shuffle();
    saveToImage(perlin.getPixels(), convert.str());
  }

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

  GeneratedImage dilatation = testCircle;
  sf::Clock dilatationTime;
  dilatation.smoothDilatation(20);
  std::cout << "Dilatation time (20): " << dilatationTime.getElapsedTime().asMilliseconds() << '\n';
  saveToImage(dilatation.getPixels(), "test_circle_smooth_dilatation.png");

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
    std::cout << "OK - Save/Loading of a generated image" << '\n';
  else
    std::cout << "FAILED - Save/Loading of a generated image" << '\n';

  remove("testSave.png");
}

void TestHandler::runTests(const Controller& controller) const {
  std::cout << "Initialization time: " << _beginningOfProg.getElapsedTime().asMilliseconds() << '\n';
  displayGameGeneratedComponents(controller.getGame());
  testImageHandling();
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
