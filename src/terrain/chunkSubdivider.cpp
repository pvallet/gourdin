#include "chunkSubdivider.h"

#include "chunk.h"

ChunkSubdivider::ChunkSubdivider ():
  _computingThread(&ChunkSubdivider::executeTasks, std::ref(*this)) {}

void ChunkSubdivider::executeTasks() {
  while (true) {
    if (!_taskQueue.empty()) {
      Task task = _taskQueue.front();

      task.chunk->generateSubdivisionLevel(task.subdivLvl);

      _mutexQueue.lock();
      _taskQueue.pop();
      _mutexQueue.unlock();
    }
  }
}

void ChunkSubdivider::addTask(Chunk* chunk, size_t subdivLvl) {
  _mutexQueue.lock();
  _taskQueue.push({chunk, subdivLvl});
  _mutexQueue.unlock();
}
