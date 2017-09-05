#include "chunkSubdivider.h"

#include "chunk.h"

ChunkSubdivider::ChunkSubdivider ():
  _computingThread(&ChunkSubdivider::executeTasks, this) {}

void ChunkSubdivider::executeTasks() {
  std::unique_lock<std::mutex> lock(_mutexCVQueueNotEmpty);
  while (true) {
    while (_taskQueue.empty()) {
      _cvQueueNotEmpty.wait(lock);
    }

    Task task = _taskQueue.front();

    task.chunk->generateSubdivisionLevel(task.subdivLvl);

    _mutexQueue.lock();
    _taskQueue.pop();
    _mutexQueue.unlock();
  }
}

void ChunkSubdivider::addTask(Chunk* chunk, size_t subdivLvl) {
  _mutexQueue.lock();
  _taskQueue.push({chunk, subdivLvl});
  _mutexQueue.unlock();

  if (_taskQueue.size() == 1) {
    std::unique_lock<std::mutex> lock(_mutexCVQueueNotEmpty);
    _cvQueueNotEmpty.notify_one();
  }
}
