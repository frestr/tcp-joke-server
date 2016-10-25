CXX=g++
CPPFLAGS=-std=c++11 -pthread

SRCS=$(wildcard src/*.cpp)

all: server

server: $(SRCS)
	$(CXX) $(CPPFLAGS) -o server $(SRCS)
