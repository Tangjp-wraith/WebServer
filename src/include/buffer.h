/**
 * @file buffer.h
 * @author Tang Jiapeng (tangjiapeng0215@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-02-01
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef BUFFER_H
#define BUFFER_H

#include <assert.h>
#include <sys/uio.h>
#include <unistd.h>

#include <atomic>
#include <cstring>
#include <iostream>
#include <vector>

class Buffer {
 public:
  Buffer(int init_buffer_size = 1024);
  ~Buffer() = default;

  size_t WriteableBytes() const;
  size_t ReadableBytes() const;
  size_t PrependableBytes() const;

  const char* Peek() const;
  void EnsureWriteable(size_t len);
  void HasWritten(size_t len);

  void Retrieve(size_t len);
  void RetrieveUntil(const char* end);

  void RetrieveAll();
  std::string RetrieveAllToStr();

  const char* BeginWriteConst() const;
  char* BeginWrite();

  void Append(const std::string& str);
  void Append(const char* str, size_t len);
  void Append(const void* data, size_t len);
  void Append(const Buffer& buff);

  size_t ReadFd(int fd, int* saveErrno);
  size_t WriteFd(int fd, int* saveErrno);

 private:
  char* BeginPtr();
  const char* BeginPtr() const;
  void MakeSpace(size_t len);

  std::vector<char> buffer_;
  std::atomic<std::size_t> read_pos_;
  std::atomic<std::size_t> wirte_pos_;
};

#endif //BUFFER_H