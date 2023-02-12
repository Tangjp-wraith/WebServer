/**
 * @file httpconn.h
 * @author Tang Jiapeng (tangjiapeng0215@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-02-10
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef HTTP_CONN_H
#define HTTP_CONN_H

#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>

#include "../buffer/buffer.h"
#include "../log/log.h"
#include "../pool/sqlconnRAII.h"
#include "httprequest.h"
#include "httpresponse.h"

class HttpConn {
 public:
  HttpConn();
  ~HttpConn();
  void init(int sockFd, const sockaddr_in &addr);
  ssize_t read(int *saveErrno);
  ssize_t write(int *saveErrno);
  void Close();
  int GetFd() const;
  const char *GetIP() const;
  int GetPort() const;
  sockaddr_in GetAddr() const;
  bool process();
  int ToWriteBytes();
  bool IsKeepAlive() const;

  static bool is_et_;
  static const char *src_dir_;
  static std::atomic<int> user_count_;

 private:
  int fd_;
  struct sockaddr_in addr_;
  bool is_close_;
  int iov_cnt_;
  struct iovec iov_[2];
  Buffer read_buff_;
  Buffer write_buff_;
  HttpRequest request_;
  HttpResponse response_;
};

#endif