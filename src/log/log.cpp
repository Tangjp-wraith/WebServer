/**
 * @file log.cpp
 * @author Tang Jiapeng (tangjiapeng0215@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-02-02
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "log.h"

void Log::init(int level = 1, const char* path, const char* suffix,
               int maxQueueCapacity) {
  is_open_ = true;
  level = level_;
  if (maxQueueCapacity > 0) {
    is_async_ = true;
    if (!deque_) {
      std::unique_ptr<BlockQueue<std::string>> newDeque(
          new BlockQueue<std::string>);
      deque_ = std::move(newDeque);
      std::unique_ptr<std::thread> newThread(new std::thread(FlushLogThread));
      write_thread_ = std::move(newThread);
    }
  } else {
    is_async_ = false;
  }
  line_count_ = 0;
  time_t timer = time(nullptr);
  struct tm* sysTime = localtime(&timer);
  struct tm t = *sysTime;
  path_ = path;
  suffix_ = suffix;
  char fileName[LOG_NAME_LEN] = {0};
  snprintf(fileName, LOG_NAME_LEN - 1, "%s/%04d_%02d_%02d%s", path_,
           t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, suffix_);
  today_ = t.tm_mday;
  {
    std::lock_guard<std::mutex> locker(mtx_);
    buffer_.RetrieveAll();
    if (fp_) {
      flush();
      fclose(fp_);
    }
    fp_ = fopen(fileName, "a");
    if (fp_ == nullptr) {
      mkdir(path_, 0777);
      fp_ = fopen(fileName, "a");
    }
    assert(fp_ != nullptr);
  }
}

Log* Log::Instance() {
  static Log instance;
  return &instance;
}

void Log::FlushLogThread() { Log::Instance()->AsyncWrite(); }

void Log::write(int level, const char* format, ...) {
  struct timeval now = {0, 0};
  gettimeofday(&now, nullptr);
  time_t tSec = now.tv_sec;
  struct tm* sysTime = localtime(&tSec);
  struct tm t = *sysTime;
  va_list vaList;
  if (today_ != t.tm_mday || (line_count_ && (line_count_ % MAX_LINES == 0))) {
    std::unique_lock<std::mutex> locker(mtx_);
    locker.unlock();
    char newFile[LOG_NAME_LEN];
    char tail[36] = {0};
    snprintf(tail, 36, "%04d_%02d_%02d", t.tm_year + 1900, t.tm_mon + 1,
             t.tm_mday);
    if (today_ != t.tm_mday) {
      snprintf(newFile, LOG_NAME_LEN - 72, "%s/%s%s", path_, tail, suffix_);
      today_ = t.tm_mday;
      line_count_ = 0;
    } else {
      snprintf(newFile, LOG_NAME_LEN - 72, "%s/%s-%d%s", path_, tail,
               (line_count_ / MAX_LINES), suffix_);
    }
    locker.lock();
    flush();
    fclose(fp_);
    fp_ = fopen(newFile, "a");
    assert(fp_ != nullptr);
  }
  {
    std::unique_lock<std::mutex> locker(mtx_);
    ++line_count_;
    int n = snprintf(buffer_.BeginWrite(), 128,
                     "%d-%02d-%02d %02d:%02d:%02d.%06ld ", t.tm_year + 1900,
                     t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec,
                     now.tv_usec);

    buffer_.HasWritten(n);
    AppendLogLevelTitle(level);

    va_start(vaList, format);
    int m = vsnprintf(buffer_.BeginWrite(), buffer_.WriteableBytes(), format,
                      vaList);
    va_end(vaList);

    buffer_.HasWritten(m);
    buffer_.Append("\n\0", 2);

    if (is_async_ && deque_ && !deque_->full()) {
      deque_->push_back(buffer_.RetrieveAllToStr());
    } else {
      fputs(buffer_.Peek(), fp_);
    }
    buffer_.RetrieveAll();
  }
}

void Log::flush() {
  if (is_async_) {
    deque_->flush();
  }
  fflush(fp_);
}

int Log::GetLevel() {
  std::lock_guard<std::mutex> locker(mtx_);
  return level_;
}

void Log::SetLevel(int level) {
  std::lock_guard<std::mutex> locker(mtx_);
  level_ = level;
}

bool Log::IsOpen() { return is_open_; }

void Log::AppendLogLevelTitle(int level) {
  switch (level) {
    case 0:
      buffer_.Append("[debug]: ", 9);
      break;
    case 1:
      buffer_.Append("[info] : ", 9);
      break;
    case 2:
      buffer_.Append("[warn] : ", 9);
      break;
    case 3:
      buffer_.Append("[error]: ", 9);
      break;
    default:
      buffer_.Append("[debug]: ", 9);
      break;
  }
}

Log::Log()
    : line_count_(0),
      today_(0),
      is_async_(false),
      fp_(nullptr),
      deque_(nullptr),
      write_thread_(nullptr) {}

Log::~Log() {
  if (write_thread_ && write_thread_->joinable()) {
    while (!deque_->empty()) {
      deque_->flush();
    }
    deque_->close();
    write_thread_->join();
  }
  if (fp_) {
    std::lock_guard<std::mutex> locker(mtx_);
    flush();
    fclose(fp_);
  }
}

void Log::AsyncWrite() {
  std::string str = "";
  while (deque_->pop(str)) {
    std::lock_guard<std::mutex> locker(mtx_);
    fputs(str.c_str(), fp_);
  }
}
