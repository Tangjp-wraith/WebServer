/**
 * @file sqkconnRAII.h
 * @author Tang Jiapeng (tangjiapeng0215@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-02-10
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef SQLCONNRAII_H
#define SQLCONNRAII_H

#include "sqlconnpool.h"

class SqlConnRAII {
 public:
  SqlConnRAII(MYSQL **sql, SqlConnPool *connpool);
  ~SqlConnRAII();

 private:
  MYSQL *sql_;
  SqlConnPool *connpool_;
};

#endif

SqlConnRAII::SqlConnRAII(MYSQL **sql, SqlConnPool *connpool) {
  assert(connpool);
  *sql = connpool->GetConn();
  sql_ = *sql;
  connpool_ = connpool;
}

SqlConnRAII::~SqlConnRAII() {
  if (sql_) {
    connpool_->FreeConn(sql_);
  }
}