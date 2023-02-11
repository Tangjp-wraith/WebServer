/**
 * @file httprequest.h
 * @author Tang Jiapeng (tangjiapeng0215@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-02-10
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <errno.h>
#include <mysql/mysql.h>

#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "../log/log.h"
#include "../pool/sqkconnRAII.h"
#include "../pool/sqlconnpool.h"

class HttpRequest {
 public:
  enum PARSE_STATE {
    REQUEST_LINE,
    HEADERS,
    BODY,
    FINISH,
  };

  enum HTTP_CODE {
    NO_REQUEST = 0,
    GET_REQUEST,
    BAD_REQUEST,
    NO_RESOURSE,
    FORBIDDENT_REQUEST,
    FILE_REQUEST,
    INTERNAL_ERROR,
    CLOSED_CONNECTION,
  };

  HttpRequest();
  ~HttpRequest() = default;
  void Init();
  bool parse(Buffer &buff);

  std::string path() const;
  std::string &path();
  std::string method() const;
  std::string version() const;
  std::string GetPost(const std::string &key) const;
  std::string GetPost(const char *key) const;

  bool IsKeepAlive() const;

 private:
  bool ParseRequestLine(const std::string &line);
  void ParseHeader(const std::string &line);
  void ParseBody(const std::string &line);

  void ParsePath();
  void ParsePost();
  void ParseFromUrlEncoded();

  static bool UserVerify(const std::string &name, const std::string &pwd,
                        bool isLogin);

  PARSE_STATE state_;
  std::string method_;
  std::string path_;
  std::string version_;
  std::string body_;
  std::unordered_map<std::string, std::string> header_;
  std::unordered_map<std::string, std::string> post_;

  static const std::unordered_set<std::string> DEFAULT_HTML_;
  static const std::unordered_map<std::string, int> DEFAULT_HTML_TAG_;
  static int ConverHex(char ch);
};

#endif  // HTTP_REQUEST_H
