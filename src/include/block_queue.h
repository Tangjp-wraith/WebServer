/**
 * @file block_queue.h
 * @author Tang Jiapeng (tangjiapeng0215@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-01-31
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef BLOCK_QUEUE_H
#define BLOCK_QUEUE_H

#include <sys/time.h>
#include <assert.h>
#include <condition_variable>
#include <deque>
#include <mutex>

template <class T>
class BlockQueue {
 public:
  explicit BlockQueue(size_t MaxCapatity = 1024);
  ~BlockQueue();
  void clear();
  bool empty();
  bool full();
  void close();
  size_t size();
  size_t capacity();
  T front();
  T back();
  void push_back(const T& item);
  void push_front(const T& item);
  bool pop(T& item);
  bool pop(T& item, int timeout);
  void flush();

 private:
  std::deque<T> deq_;
  size_t capacity_;
  std::mutex mtx_;
  bool is_close_;
  std::condition_variable consumer_;
  std::condition_variable producer_;
};

template <class T>
BlockQueue<T>::BlockQueue(size_t MaxCapacity)
    : capacity_(MaxCapacity), is_close_(false) {
  assert(MaxCapacity > 0);
}

template <class T>
BlockQueue<T>::~BlockQueue() {
  close();
}

template <class T>
void BlockQueue<T>::close() {
  {
    std::lock_guard<std::mutex> locker(mtx_);
    deq_.clear();
    is_close_ = true;
  }
  consumer_.notify_all();
  producer_.notify_all();
}

template <class T>
void BlockQueue<T>::flush() {
  consumer_.notify_one();
}

template <class T>
void BlockQueue<T>::clear() {
  std::lock_guard<std::mutex> locker(mtx_);
  deq_.clear();
}

template <class T>
T BlockQueue<T>::front() {
  std::lock_guard<std::mutex> locker(mtx_);
  return deq_.front();
}

template <class T>
T BlockQueue<T>::back() {
  std::lock_guard<std::mutex> locker(mtx_);
  return deq_.back();
}

template <class T>
bool BlockQueue<T>::empty() {
  std::lock_guard<std::mutex> locker(mtx_);
  return deq_.empty();
}

template <class T>
bool BlockQueue<T>::full() {
  std::lock_guard<std::mutex> locker(mtx_);
  return deq_.size() >= capacity_;
}

template <class T>
size_t BlockQueue<T>::size() {
  std::lock_guard<std::mutex> locker(mtx_);
  return deq_.size();
}

template <class T>
size_t BlockQueue<T>::capacity() {
  std::lock_guard<std::mutex> locker(mtx_);
  return capacity_;
}

template <class T>
void BlockQueue<T>::push_back(const T& item) {
  std::unique_lock<std::mutex> locker(mtx_);
  while (deq_.size() >= capacity_) {
    producer_.wait(locker);
  }
  deq_.push_back(item);
  consumer_.notify_one();
}

template <class T>
void BlockQueue<T>::push_front(const T& item) {
  std::unique_lock<std::mutex> locker(mtx_);
  while (deq_.size() >= capacity_) {
    producer_.wait(locker);
  }
  deq_.push_front(item);
  consumer_.notify_one();
}

template <class T>
bool BlockQueue<T>::pop(T& item) {
  std::unique_lock<std::mutex> locker(mtx_);
  while (deq_.empty()) {
    consumer_.wait(locker);
    if (is_close_) {
      return false;
    }
  }
  item = deq_.front();
  deq_.pop_front();
  producer_.notify_one();
  return true;
}

template <class T>
bool BlockQueue<T>::pop(T& item, int timeout) {
  std::unique_lock<std::mutex> locker(mtx_);
  while (deq_.empty()) {
    if (is_close_ ||
        consumer_.wait_for(locker, std::chrono::seconds(timeout)) ==
            std::cv_status::timeout) {
      return false;
    }
  }
  item = deq_.front();
  deq_.pop_front();
  producer_.notify_one();
  return true;
}

#endif  // BLOCK_QUEUE_H
