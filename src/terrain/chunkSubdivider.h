#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

class Chunk;

struct Task {
  Chunk* chunk;
  size_t subdivLvl;
};

class ChunkSubdivider {
public:
  ChunkSubdivider ();

  void addTask(Chunk* chunk, size_t subdivLvl);

private:
  void executeTasks();

  std::condition_variable _cvQueueNotEmpty;
  std::mutex _mutexCVQueueNotEmpty;
  std::mutex _mutexQueue;
  std::thread _computingThread;
  std::queue<Task> _taskQueue;
};
