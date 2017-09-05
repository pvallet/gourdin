#include "chunkSubdivider.h"

#include "chunk.h"

ChunkSubdivider::ChunkSubdivider ():
  _computingThread(&ChunkSubdivider::executeTasks, this) {}

void ChunkSubdivider::executeTasks() {
  while (true) {
    std::unique_lock<std::mutex> lockQueue(_mutexQueue);
    while (_taskQueue.empty()) {
      _cvQueueNotEmpty.wait(lockQueue);
    }
    lockQueue.unlock();

    Task task = _taskQueue.front();

    task.chunk->generateSubdivisionLevel(task.subdivLvl);

    lockQueue.lock();
    _taskQueue.pop();
    if (_taskQueue.size() == 0)
      _cvTasksCompleted.notify_one();
  }
}

void ChunkSubdivider::waitForTasksToFinish() {
  std::unique_lock<std::mutex> lockQueue(_mutexQueue);
  while (!_taskQueue.empty()) {
    _cvTasksCompleted.wait(lockQueue);
  }
}

void ChunkSubdivider::addTask(Chunk* chunk, size_t subdivLvl) {
  std::unique_lock<std::mutex> lockQueue(_mutexQueue);
  _taskQueue.push({chunk, subdivLvl});

  if (_taskQueue.size() == 1) {
    _cvQueueNotEmpty.notify_one();
  }
}
