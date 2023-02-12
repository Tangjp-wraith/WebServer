/**
 * @file httpconn.cpp
 * @author Tang Jiapeng (tangjiapeng0215@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-02-12
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "httpconn.h"

const char* HttpConn::src_dir_;
std::atomic<int> HttpConn::user_count_;
bool HttpConn::is_et_;

HttpConn::HttpConn() : fd_(-1), addr_({0}), is_close_(true) {}

HttpConn::~HttpConn() { Close(); }

void HttpConn::init(int sockFd, const sockaddr_in& addr) {
  assert(fd_ > 0);
  user_count_++;
  addr_ = addr;
  fd_ = sockFd;
  write_buff_.RetrieveAll();
  read_buff_.RetrieveAll();
  is_close_ = false;
  LOG_INFO("Clinet[%d](%s:%d) in, userCount:%d", fd_, GetIP(), GetPort(),
           (int)user_count_);
}

ssize_t HttpConn::read(int* saveErrno) {
  ssize_t len = -1;
  do {
    len = read_buff_.ReadFd(fd_, saveErrno);
    if (len <= 0) {
      break;
    }
  } while (is_et_);
  return len;
}

ssize_t HttpConn::write(int* saveErrno) {
  ssize_t len = -1;
  do {
    len = writev(fd_, iov_, iov_cnt_);
    if (len <= 0) {
      *saveErrno = errno;
      break;
    }
    if (iov_[0].iov_len + iov_[1].iov_len == 0) {
      break;
    } else if (static_cast<size_t>(len) > iov_[0].iov_len) {
      iov_[1].iov_base = (uint8_t*)iov_[1].iov_base + (len - iov_[0].iov_len);
      iov_[1].iov_len -= (len - iov_[0].iov_len);
      if (iov_[0].iov_len) {
        write_buff_.RetrieveAll();
        iov_[0].iov_len = 0;
      }
    } else {
      iov_[0].iov_base = (uint8_t*)iov_[0].iov_base + len;
      iov_[0].iov_len -= len;
      write_buff_.Retrieve(len);
    }
  } while (is_et_ || ToWriteBytes() > 10240);
  return len;
}

void HttpConn::Close() {
  response_.UnmapFile();
  if (!is_close_) {
    is_close_ = true;
    --user_count_;
    close(fd_);
    LOG_INFO("Clinet[%d](%s:%d) quit, userCount:%d", fd_, GetIP(), GetPort(),
             (int)user_count_);
  }
}

int HttpConn::GetFd() const { return fd_; }

const char* HttpConn::GetIP() const { return inet_ntoa(addr_.sin_addr); }

int HttpConn::GetPort() const { return addr_.sin_port; }

sockaddr_in HttpConn::GetAddr() const { return addr_; }

bool HttpConn::process() {
  request_.Init();
  if (read_buff_.ReadableBytes() <= 0) {
    return false;
  } else if (request_.parse(read_buff_)) {
    LOG_DEBUG("%s", request_.path().c_str());
    response_.Init(src_dir_, request_.path(), request_.IsKeepAlive(), 200);
  } else {
    response_.Init(src_dir_, request_.path(), false, 400);
  }
  response_.MakeResponse(write_buff_);
  iov_[0].iov_base = const_cast<char*>(write_buff_.Peek());
  iov_[0].iov_len = write_buff_.ReadableBytes();
  iov_cnt_ = 1;
  if (response_.FileLen() > 0 && response_.File()) {
    iov_[1].iov_base = response_.File();
    iov_[1].iov_len = response_.FileLen();
    iov_cnt_ = 2;
  }
  LOG_DEBUG("filesize:%d, %d to %d", response_.FileLen(), iov_cnt_,
            ToWriteBytes());
  return true;
}

int HttpConn::ToWriteBytes() { return iov_[0].iov_len + iov_[1].iov_len; }

bool HttpConn::IsKeepAlive() const { return request_.IsKeepAlive(); }
