#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <stddef.h> // size_t
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

  void join();
  void waitForTasksToFinish();

  inline size_t getNbTasksInQueue() const {return _taskQueue.size();}

private:
  void executeTasks();

  std::atomic<bool> _continue;
  std::condition_variable _cvQueueNotEmpty;
  std::condition_variable _cvTasksCompleted;
  std::mutex _mutexQueue;
  std::queue<Task> _taskQueue;
  std::thread _computingThread;
};
