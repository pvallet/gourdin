#include "testHandler.hpp"

#include <SFML/Graphics.hpp>

#include <cstdio>
#include <fstream>

#define DELETE_LIST_NAME "to_delete"

void TestHandler::saveToImage(const std::vector<sf::Uint8>& pixels, std::string filename) {
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

void TestHandler::ContentGeneratorDisplayForestsMask(
  const ContentGenerator& contentGenerator, std::string savename) {

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

void TestHandler::PerlinSaveToImage(const Perlin& perlin, std::string savename) {
  size_t size = perlin.getSize();
  std::vector<sf::Uint8> pixels(size * size * 4, 255);

	for (int i = 0 ; i < size ; i++) { // Convert mask to array of pixels
		for (int j = 0 ; j < size ; j++) {
      float value = perlin.getValue(i, j);
      pixels[i*4*size + j*4] = value*255;
      pixels[i*4*size + j*4 + 1] = value*255;
      pixels[i*4*size + j*4 + 2] = value*255;
		}
	}

	saveToImage(pixels, savename);
}

void TestHandler::runTests(const Controller& controller) {
  const Game& game = controller.getGame();

  PerlinSaveToImage(game._reliefGenerator, "relief.png");

  Perlin perlin(3, 0.06, 0.1, 512);

  for (size_t i = 0; i < 1; i++) {
    std::ostringstream convert;
    convert << i << ".png";
    perlin.shuffle();
    PerlinSaveToImage(perlin, convert.str());
  }

  ContentGeneratorDisplayForestsMask(game._contentGenerator, "contents.png");
}

void TestHandler::clean() {
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
