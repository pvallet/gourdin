#pragma once

#include <glm/glm.hpp>
#include <list>
#include <unordered_map>
#include <vector>

struct gridHashFunc {
  size_t operator()(const glm::ivec2 &k) const {
  size_t h1 = std::hash<int>()(k.x);
  size_t h2 = std::hash<int>()(k.y);
  return (h1 << 1) + h1 + h2;
  }
};

class igMovingElement;

typedef std::vector<const std::list<igMovingElement*>* > TVecList;
typedef std::unordered_map<glm::ivec2, std::list<igMovingElement*>, gridHashFunc> TGridList;
typedef std::unordered_map<glm::ivec2, TVecList, gridHashFunc> TGridVecList;

#define FOR_TVECLIST(it, container) \
  for (size_t tveclist_i = 0; tveclist_i < container.size(); tveclist_i++) \
  for (auto it = container[tveclist_i]->begin(); it != container[tveclist_i]->end(); it++)
