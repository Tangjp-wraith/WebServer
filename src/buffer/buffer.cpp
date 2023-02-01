/**
 * @file buffer.cpp
 * @author Tang Jiapeng (tangjiapeng0215@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-02-01
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "buffer.h"

Buffer::Buffer(int init_buffer_size)
    : buffer_(init_buffer_size), read_pos_(0), wirte_pos_(0) {}

size_t Buffer::WriteableBytes() const { return buffer_.size() - wirte_pos_; }

size_t Buffer::ReadableBytes() const { return wirte_pos_ - read_pos_; }

size_t Buffer::PrependableBytes() const { return read_pos_; }

const char* Buffer::Peek() const { return BeginPtr() + read_pos_; }

void Buffer::EnsureWriteable(size_t len) {
  if (WriteableBytes() < len) {
    MakeSpace(len);
  }
  assert(WriteableBytes() >= len);
}

void Buffer::HasWritten(size_t len) { wirte_pos_ + len; }

void Buffer::Retrieve(size_t len) {
  assert(len <= ReadableBytes());
  if (len < ReadableBytes()) {
    read_pos_ += len;
  } else {
    RetrieveAll();
  }
}

void Buffer::RetrieveUntil(const char* end) {
  assert(Peek() <= end);
  assert(end <= BeginWriteConst());
  Retrieve(end - Peek());
}

void Buffer::RetrieveAll() {
  bzero(&buffer_[0], buffer_.size());
  wirte_pos_ = 0;
  read_pos_ = 0;
}

std::string Buffer::RetrieveAllToStr() {
  std::string str(Peek(), ReadableBytes());
  RetrieveAll();
  return str;
}

const char* Buffer::BeginWriteConst() const { return BeginPtr() + wirte_pos_; }

char* Buffer::BeginWrite() { return BeginPtr() + wirte_pos_; }

void Buffer::Append(const std::string& str) {
  Append(str.data(), str.length());
}

void Buffer::Append(const char* str, size_t len) {
  assert(str);
  EnsureWriteable(len);
  std::copy(str, str + len, BeginWrite());
  HasWritten(len);
}

void Buffer::Append(const void* data, size_t len) {
  assert(data);
  Append(static_cast<const char*>(data), len);
}

void Buffer::Append(const Buffer& buff) {
  Append(buff.Peek(), buff.ReadableBytes());
}

size_t Buffer::ReadFd(int fd, int* saveErrno) {
  char buff[65535];
  struct iovec iov[2];
  const size_t writeable = WriteableBytes();
  iov[0].iov_base = BeginPtr() + wirte_pos_;
  iov[0].iov_len = writeable;
  iov[1].iov_base = buff;
  iov[1].iov_len = sizeof(buff);
  const ssize_t len = readv(fd, iov, 2);
  if (len < 0) {
    *saveErrno = errno;
  } else if (static_cast<size_t>(len) <= writeable) {
    wirte_pos_ += len;
  } else {
    wirte_pos_ = buffer_.size();
    Append(buff, len - writeable);
  }
  return len;
}

size_t Buffer::WriteFd(int fd, int* saveErrno) {
  size_t readable=ReadableBytes();
  ssize_t len=write(fd,Peek(),readable);
  if(len<0){
    *saveErrno=errno;
    return len;
  }
  read_pos_+=len;
  return len;
}

char* Buffer::BeginPtr() { return &*buffer_.begin(); }

const char* Buffer::BeginPtr() const { return &*buffer_.begin(); }

void Buffer::MakeSpace(size_t len) {
  if (WriteableBytes() + PrependableBytes() < len) {
    buffer_.resize(wirte_pos_ + len + 1);
  } else {
    size_t readable = ReadableBytes();
    std::copy(BeginPtr() + read_pos_, BeginPtr() + wirte_pos_, BeginPtr());
    read_pos_ = 0;
    wirte_pos_ = read_pos_ + readable;
    assert(readable == ReadableBytes());
  }
}
