/**
 * @file httpresponse.h
 * @author Tang Jiapeng (tangjiapeng0215@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-02-11
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <unordered_map>

#include "../buffer/buffer.h"
#include "../log/log.h"

class HttpResponse {
 public:
  HttpResponse();
  ~HttpResponse();

  void Init(const std::string &str_dir, std::string &path,
            bool is_keep_alive = false, int code = -1);
  void MakeResponse(Buffer &buff);
  void UnmapFile();
  char *File();
  size_t FileLen() const;
  void ErrorContent(Buffer &buff, std::string message);
  int Code() const;

 private:
  void AddStateLine(Buffer &buff);
  void AddHeader(Buffer &buff);
  void AddContent(Buffer &buff);

  void ErrorHtml();
  std::string GetFileType();

  int code_;
  bool is_keep_alive_;

  std::string path_;
  std::string src_dir_;

  char *mm_file_;
  struct stat mm_file_stat_;

  static const std::unordered_map<std::string, std::string> SUFFIX_TYPE_;
  static const std::unordered_map<int, std::string> CODE_STATUS_;
  static const std::unordered_map<int, std::string> CODE_PATH_;
};

#endif  // HTTP_RESPONSE_H