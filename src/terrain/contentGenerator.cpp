#include "contentGenerator.h"

#include <cstdlib>
#include <sstream>

#include "antilope.h"
#include "lion.h"

#define CONTENT_RES 512

#define MIN_ANTILOPE_PROX 5.f
#define HERD_RADIUS 10.f // for 10 antilopes

ContentGenerator::ContentGenerator() :
  _perlinGenerator(rand(), CONTENT_RES) {}

void ContentGenerator::init() {
  _antilopeTexManager.load("res/animals/antilope/");
  _lionTexManager.    load("res/animals/lion/");
  _treeTexManager.    load("res/trees/");
}

void ContentGenerator::saveToImage(std::string savename) {
  std::vector<sf::Uint8> pixels(CONTENT_RES * CONTENT_RES * 4);

	for (int i = 0 ; i < CONTENT_RES ; i++) { // Convert heights to array of pixels
		for (int j = 0 ; j < CONTENT_RES ; j++) {
			for (int k = 0 ; k < 3 ; k++) {
				pixels[i*4*CONTENT_RES + j*4 + k] = 0;
			}
			pixels[i*4*CONTENT_RES + j*4 + 3] = 255;
		}
	}

	sf::Texture texture;
	texture.create(CONTENT_RES, CONTENT_RES);
	texture.update(&pixels[0]);

  std::ostringstream convert;
  convert << savename << ".png";

	texture.copyToImage().saveToFile(convert.str());
}

// void Game::genForests(size_t x, size_t y) {
//   float r, theta;
//   sf::Vector2f p, diff;
//   bool add;
//   size_t count, nbTrees;
//
//   std::vector<Tree*> res;
//
//   std::vector<Center*> centers = _map.getCentersInChunk(x,y);
//
//   for (unsigned int i = 0 ; i < centers.size() ; i++) {
//     if (centers[i]->biome >= 11) { // No forests in other biomes
//       count = 0;
//       res.clear();
//       nbTrees = _treeTexManager.getExtension(centers[i]->biome);
//       nbTrees *= 1.5;
//
//       for (size_t j = 0 ; j < nbTrees ; j++) {
//         add = true;
//         r = sqrt(RANDOMF) * _treeTexManager.getExtension(centers[i]->biome) * sqrt(nbTrees);
//         theta = RANDOMF * 2*M_PI;
//
//         p.x = centers[i]->x + r*cos(theta);
//         p.y = centers[i]->y + r*sin(theta);
//
//         for (unsigned int k = 0 ; k < res.size() ; k++) {
//           diff = res[k]->getPos() - p;
//
//           if (diff.x * diff.x + diff.y * diff.y < _treeTexManager.getDensity(centers[i]->biome) ||
//             _map.getClosestCenter(p)->biome != centers[i]->biome) {
//             add = false;
//           }
//         }
//
//         if (add) {
//           count++;
//           res.push_back(new Tree(p, _treeTexManager, centers[i]->biome,
//             (int) ((RANDOMF - 0.01f) * _treeTexManager.getNBTrees(centers[i]->biome))));
//         }
//       }
//     }
//   }
// }

std::vector<igElement*> ContentGenerator::genHerd(sf::Vector2f pos, size_t count) {
  float r, theta;
  sf::Vector2f p, diff;
  bool add;

  std::vector<igElement*> res;

  for (size_t i = 0 ; i < count ; i++) {
    add = true;
    r = sqrt(RANDOMF) * HERD_RADIUS * sqrt(count);
    theta = RANDOMF * 2*M_PI;

    p.x = pos.x + r*cos(theta);
    p.y = pos.y + r*sin(theta);

    for (unsigned int j = 0 ; j < res.size() ; j++) {
      diff = res[j]->getPos() - p;

      if (diff.x * diff.x + diff.y * diff.y < MIN_ANTILOPE_PROX) {
        i--;
        add = false;
      }
    }

    if (add) {
      res.push_back(new Antilope(p, AnimationManager(_antilopeTexManager)));
    }
  }

  return res;
}

std::vector<igElement*> ContentGenerator::genLion(sf::Vector2f pos) {
  std::vector<igElement*> res;
  res.push_back(new Lion(pos, AnimationManager(_lionTexManager)));
  return res;
}
