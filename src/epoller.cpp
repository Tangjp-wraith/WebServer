/**
 * @file epoller.cpp
 * @author Tang Jiapeng (tangjiapeng0215@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-02-12
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "include/epoller.h"

Epoller::Epoller(int maxEvent)
    : epoll_fd_(epoll_create(512)), events_(maxEvent) {
  assert(epoll_fd_ >= 0 && events_.size() > 0);
}

Epoller::~Epoller() { close(epoll_fd_); }

bool Epoller::AddFd(int fd, uint32_t events) {
  if (fd < 0) {
    return false;
  }
  epoll_event ev = {0};
  ev.data.fd = fd;
  ev.events = events;
  return 0 == epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev);
}

bool Epoller::ModFd(int fd, uint32_t events) {
  if (fd < 0) {
    return false;
  }
  epoll_event ev = {0};
  ev.data.fd = fd;
  ev.events = events;
  return 0 == epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ev);
}

bool Epoller::DelFd(int fd) {
  if (fd < 0) {
    return false;
  }
  epoll_event ev = {0};
  return 0 == epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &ev);
}

int Epoller::Wait(int timeoutMs) {
  return epoll_wait(epoll_fd_, &events_[0], static_cast<int>(events_.size()),
                    timeoutMs);
}

int Epoller::GetEventFd(size_t index) const {
  assert(index < events_.size() && index >= 0);
  return events_[index].data.fd;
}

uint32_t Epoller::GetEvents(size_t index) const {
  assert(index < events_.size() && index >= 0);
  return events_[index].events;
}
