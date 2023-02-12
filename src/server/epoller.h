/**
 * @file epoller.h
 * @author Tang Jiapeng (tangjiapeng0215@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-02-12
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef EPOLLER_H
#define EPOLLER_H

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <vector>

class Epoller {
 public:
  explicit Epoller(int maxEvent = 1024);
  ~Epoller();
  bool AddFd(int fd, uint32_t events);
  bool ModFd(int fd, uint32_t events);
  bool DelFd(int fd);
  int Wait(int timeoutMs = -1);
  int GetEventFd(size_t index) const;
  uint32_t GetEvents(size_t index) const;

 private:
  int epoll_fd_;
  std::vector<struct epoll_event> events_;
};

#endif