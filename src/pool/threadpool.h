/**
 * @file threadpool.h
 * @author Tang Jiapeng (tangjiapeng0215@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-02-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <assert.h>

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

class ThreadPool {
 public:
  explicit ThreadPool(size_t threadCount = 8);
  ThreadPool() = default;
  ThreadPool(ThreadPool&&) = default;
  ~ThreadPool();

  template <class F>
  void AddTask(F&& task);

 private:
  struct Pool {
    std::mutex mtx_;
    std::condition_variable cond_;
    bool is_close_;
    std::queue<std::function<void()>> tasks_;
  };
  std::shared_ptr<Pool> pool_;
};

#endif

inline ThreadPool::ThreadPool(size_t threadCount)
    : pool_(std::shared_ptr<Pool>()) {
  assert(threadCount > 0);
  for (size_t i = 0; i < threadCount; ++i) {
    std::thread([pool = pool_] {
      std::unique_lock<std::mutex> locker(pool->mtx_);
      while (true) {
        if (pool->tasks_.empty()) {
          auto task = std::move(pool->tasks_.front());
          pool->tasks_.pop();
          locker.unlock();
          task();
          locker.lock();
        } else if (pool->is_close_) {
          break;
        } else {
          pool->cond_.wait(locker);
        }
      }
    }).detach();
  }
}

inline ThreadPool::~ThreadPool() {
  {
    std::lock_guard<std::mutex> locker(pool_->mtx_);
    pool_->is_close_ = true;
  }
  pool_->cond_.notify_all();
}

template <class F>
inline void ThreadPool::AddTask(F&& task) {
  {
    std::lock_guard<std::mutex> locker(pool_->mtx_);
    pool_->tasks_.emplace(std::forward<F>(task));
  }
  pool_->cond_.notify_one();
}
