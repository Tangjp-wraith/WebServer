CXX = g++
CFLAGS = -std=c++14 -O2 -Wall -g 

TARGET = test

OBJS = ../src/log/*.cpp ../test/test.cpp
 
all: $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o $(TARGET)  -pthread -lmysqlclient

clean:
	rm -rf ../bin/$(OBJS) $(TARGET)
