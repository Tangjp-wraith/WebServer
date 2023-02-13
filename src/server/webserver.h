/**
 * @file webserver.h
 * @author Tang Jiapeng (tangjiapeng0215@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-02-13
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <unordered_map>

#include "../http/httpconn.h"
#include "../log/log.h"
#include "../pool/sqlconnRAII.h"
#include "../pool/sqlconnpool.h"
#include "../pool/threadpool.h"
#include "../timer/heaptimer.h"
#include "epoller.h"

class WebServer {
 public:
  WebServer(int port, int trig_mode, int timeout_ms, bool opt_linger,
            int sql_port, const char *sql_user, const char *sql_pwd,
            const char *db_name, int connpool_num, int thread_num,
            bool open_log, int log_level, int log_que_size);
  ~WebServer();
  void Start();
  
 private:
  bool InitSocket();
  void InitEventMode(int trig_mode);
  void AddClient(int fd, sockaddr_in addr);

  void DealListen();
  void DealWrite(HttpConn *client);
  void DealRead(HttpConn *client);

  void SendError(int fd, const char *info);
  void ExtentTime(HttpConn *client);
  void CloseConn(HttpConn *client);

  void OnRead(HttpConn *client);
  void OnWrite(HttpConn *client);
  void OnProcess(HttpConn *client);

  static const int MAX_FD = 65536;

  static int SetFdNonblock(int fd);

  int port_;
  bool open_linger_;
  int timeout_ms_;
  bool is_close_;
  int listen_fd_;
  char *src_dir_;
  uint32_t listen_event_;
  uint32_t conn_event_;
  std::unique_ptr<HeapTimer> timer_;
  std::unique_ptr<ThreadPool> threadpool_;
  std::unique_ptr<Epoller> epoller_;
  std::unordered_map<int, HttpConn> users_;
};

#endif