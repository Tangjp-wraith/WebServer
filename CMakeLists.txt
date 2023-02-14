cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_SHARED_LIBS ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(WebServer
  DESCRIPTION "WebServer"
  LANGUAGES C CXX
)

set(WEBSERVER_BUILD_SUPPORT_DIR "${CMAKE_SOURCE_DIR}/build")
set(WEBSERVER_CLANG_SEARCH_PATH "/usr/local/bin" "/usr/bin")

set(WEBSERVER_SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/include)

include_directories(${WEBSERVER_SRC_INCLUDE_DIR} /usr/include/mysql)

find_package(Threads)

add_executable(server
  src/buffer.cpp
  src/epoller.cpp
  src/heaptimer.cpp
  src/httpconn.cpp
  src/httprequest.cpp
  src/httpresponse.cpp
  src/log.cpp
  src/epoller.cpp
  src/webserver.cpp
  src/sqlconnpool.cpp
  src/main.cpp
)

target_link_libraries(server
  ${CMAKE_THREAD_LIBS_INIT}
  mysqlclient
)