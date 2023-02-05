/**
 * @file heaptimer.h
 * @author Tang Jiapeng (tangjiapeng0215@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-02-04
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef HEAP_TIMER_H
#define HEAP_TIMER_H

#include <arpa/inet.h>
#include <assert.h>
#include <time.h>

#include <algorithm>
#include <chrono>
#include <functional>
#include <queue>
#include <unordered_map>

#include "../log/log.h"

typedef std::function<void()> TimeoutCallBack;
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds MS;
typedef Clock::time_point TimeStamp;

struct TimerNode {
  int id;
  TimeStamp expires;
  TimeoutCallBack cb;
  bool operator<(const TimerNode& t) { return expires < t.expires; }
};

class HeapTimer {
 public:
  HeapTimer();
  ~HeapTimer();
  void adjust(int id, int timeout);
  void add(int id, int timeout, const TimeoutCallBack& cb);
  void doWork(int id);
  void clear();
  void tick();
  void pop();
  int GetNextTick();

 private:
  void del(size_t index);
  void siftup(size_t index);
  bool siftdown(size_t index, size_t n);
  void SwapNode(size_t i, size_t j);
  std::vector<TimerNode> heap_;
  std::unordered_map<int, size_t> ref_;
};

#endif
