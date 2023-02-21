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

find_package(GTest REQUIRED)


############ make server ##############
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

#######################################
##########      GTest      ############    
#######################################

############# log_test ################
add_executable(test_log
  src/log.cpp
  src/buffer.cpp
  test/log_test.cpp
)

target_link_libraries(test_log
  GTest::GTest
  GTest::Main
)

########### buffer_test ###############
add_executable(test_buffer
  src/buffer.cpp
  test/buffer_test.cpp
)

target_link_libraries(test_buffer
  GTest::GTest
  GTest::Main
)

####### block_queue_test ##############
add_executable(test_block_queue
  test/block_queue_test.cpp
)

target_link_libraries(test_block_queue
  GTest::GTest
  GTest::Main
)

# ###### heaptimer_test ################
add_executable(test_heaptimer
  src/heaptimer.cpp
  test/heaptimer_test.cpp
)

target_link_libraries(test_heaptimer
  GTest::GTest
  GTest::Main
)
