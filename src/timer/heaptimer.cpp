/**
 * @file heaptimer.cpp
 * @author Tang Jiapeng (tangjiapeng0215@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-02-05
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "heaptimer.h"

HeapTimer::HeapTimer() { heap_.reserve(64); }

HeapTimer::~HeapTimer() { clear(); }

void HeapTimer::adjust(int id, int timeout) {
  assert(!heap_.empty() && ref_.count(id) > 0);
  heap_[ref_[id]].expires=Clock::now()+MS(timeout);
  siftdown(ref_[id],heap_.size());
}

void HeapTimer::add(int id, int timeout, const TimeoutCallBack& cb) {
  assert(id >= 0);
  size_t index;
  if (ref_.count(id) == 0) {
    index = heap_.size();
    ref_[id] = index;
    heap_.push_back({id, Clock::now() + MS(timeout), cb});
    siftup(index);
  } else {
    index = ref_[id];
    heap_[index].expires = Clock::now() + MS(timeout);
    heap_[index].cb = cb;
    if (!siftdown(index, heap_.size())) {
      siftup(index);
    }
  }
}

void HeapTimer::doWork(int id) {
  if (heap_.empty() || ref_.count(id) == 0) {
    return;
  }
  size_t index = ref_[id];
  TimerNode node = heap_[index];
  node.cb();
  del(index);
}

void HeapTimer::clear() {
  heap_.clear();
  ref_.clear();
}

void HeapTimer::tick() {
  if (heap_.empty()) {
    return;
  }
  while (!heap_.empty()) {
    TimerNode node = heap_.front();
    if (std::chrono::duration_cast<MS>(node.expires - Clock::now()).count() >
        0) {
      break;
    }
    node.cb();
    pop();
  }
}

void HeapTimer::pop() {
  assert(!heap_.empty());
  del(0);
}

int HeapTimer::GetNextTick() {
  tick();
  size_t res = -1;
  if (!heap_.empty()) {
    res = std::chrono::duration_cast<MS>(heap_.front().expires - Clock::now())
              .count();
    if (res < 0) {
      res = 0;
    }
  }
  return res;
}

void HeapTimer::del(size_t index) {
  assert(!heap_.empty() && index >= 0 && index < heap_.size());
  size_t n = heap_.size() - 1;
  if (index < n) {
    SwapNode(index, n);
    if (!siftdown(index, n)) {
      siftup(index);
    }
  }
  ref_.erase(heap_.back().id);
  heap_.pop_back();
}

void HeapTimer::siftup(size_t index) {
  assert(index >= 0 && index < heap_.size());
  size_t parent = (index - 1) / 2;
  while (parent >= 0) {
    if (heap_[index] < heap_[parent]) {
      break;
    }
    SwapNode(index, parent);
    index = parent;
    parent = (index - 1) / 2;
  }
}

bool HeapTimer::siftdown(size_t index, size_t n) {
  assert(index >= 0 && index < heap_.size());
  assert(n >= 0 && n < heap_.size());
  size_t parent = index;
  size_t child = parent * 2 + 1;
  while (child < n) {
    if (child + 1 < n && heap_[child + 1] < heap_[child]) {
      child++;
    }
    if (heap_[parent] < heap_[child]) {
      break;
    }
    SwapNode(parent, child);
    parent = child;
    child = parent * 2 + 1;
  }
  return parent > index;
}

void HeapTimer::SwapNode(size_t i, size_t j) {
  assert(i >= 0 && i < heap_.size());
  assert(j >= 0 && j < heap_.size());
  std::swap(heap_[i], heap_[j]);
  ref_[heap_[i].id] = i;
  ref_[heap_[j].id] = j;
}