#pragma once

#include <queue>

class Chunk;

struct Task {
  Chunk* chunk;
  size_t subdivLvl;
};

class ChunkSubdivider {
public:
  ChunkSubdivider () {}

  inline void addTask(Chunk* chunk, size_t subdivLvl) {_taskQueue.push({chunk, subdivLvl});}

private:
  std::queue<Task> _taskQueue;
};
