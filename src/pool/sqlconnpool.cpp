/**
 * @file sqlconnpool.cpp
 * @author Tang Jiapeng (tangjiapeng0215@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-02-07
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "sqlconnpool.h"

SqlConnPool* SqlConnPool::Instance() {
  static SqlConnPool connPool;
  return &connPool;
}

MYSQL* SqlConnPool::GetConn() {
  MYSQL* sql = nullptr;
  if (conn_que_.empty()) {
    LOG_WARN("SqlConnPool busy!");
    return nullptr;
  }
  sem_wait(&sem_id_);
  {
    std::lock_guard<std::mutex> locker(mtx_);
    sql = conn_que_.front();
    conn_que_.pop();
    return sql;
  }
}

void SqlConnPool::FreeConn(MYSQL* conn) {
  assert(conn);
  std::lock_guard<std::mutex> locker(mtx_);
  conn_que_.push(conn);
  sem_post(&sem_id_);
}

int SqlConnPool::GetFreeConnCount() {
  std::lock_guard<std::mutex> locker(mtx_);
  return conn_que_.size();
}

void SqlConnPool::Init(const char* host, int port, const char* user,
                       const char* pwd, const char* dbName, int connSize = 10) {
  assert(connSize > 0);
  for (int i = 0; i < connSize; ++i) {
    MYSQL* sql = nullptr;
    sql = mysql_init(sql);
    if (!sql) {
      LOG_ERROR("MYSQL init error!");
      assert(sql);
    }
    sql = mysql_real_connect(sql, host, user, pwd, dbName, port, nullptr, 0);
    if (!sql) {
      LOG_ERROR("MYSQL connect error!");
    }
    conn_que_.push(sql);
  }
  MAX_CONN_ = connSize;
  sem_init(&sem_id_, 0, MAX_CONN_);
}

void SqlConnPool::ClosePool() {
  std::lock_guard<std::mutex> locker(mtx_);
  while (!conn_que_.empty()) {
    auto item = conn_que_.front();
    conn_que_.pop();
    mysql_close(item);
  }
  mysql_library_end();
}

SqlConnPool::SqlConnPool() : use_count_(0), free_count_(0) {}

SqlConnPool::~SqlConnPool() { ClosePool(); }
