/**
 * @file test.cpp
 * @author Tang Jiapeng (tangjiapeng0215@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "../src/log/log.h"

void test_log(){
  int cnt=0,level=0;
  Log::Instance()->init(level,"./testlog",".log",0);
  for(level=3;level>=0;level--){
    Log::Instance()->SetLevel(level);
    for(int j=0;j<10000;j++){
      for(int i=0;i<4;i++){
          LOG_BASE(i,"%s 1111111111 %d ================ ","test",cnt++);
      }
    }
  }
  cnt = 0;
    Log::Instance()->init(level, "./testlog2", ".log", 5000);
    for(level = 0; level < 4; level++) {
        Log::Instance()->SetLevel(level);
        for(int j = 0; j < 10000; j++ ){
            for(int i = 0; i < 4; i++) {
                LOG_BASE(i,"%s 222222222 %d ============= ", "Test", cnt++);
            }
        }
    }
}

int main(){
  test_log();
}