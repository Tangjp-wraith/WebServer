/**
 * @file sqlconnpool.h
 * @author Tang Jiapeng (tangjiapeng0215@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-02-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef SQLCONNPOOL_H
#define SQLCONNPOOL_H

#include <mysql/mysql.h>
#include <semaphore.h>

#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include "log.h"

class SqlConnPool {
 public:
  static SqlConnPool* Instance();

  MYSQL* GetConn();
  void FreeConn(MYSQL* conn);
  int GetFreeConnCount();

  void Init(const char* host, int port, const char* user, const char* pwd,
            const char* dbName, int connSize);
  void ClosePool();

 private:
  SqlConnPool();
  ~SqlConnPool();

  int MAX_CONN_;
  int use_count_;
  int free_count_;

  std::queue<MYSQL*> conn_que_;
  std::mutex mtx_;
  sem_t sem_id_;
};

#endif