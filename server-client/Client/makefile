CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system

all: STOMPClient
	g++ -pthread -o bin/StompClient bin/Book.o bin/connectionHandler.o bin/StompClient.o $(LDFLAGS)

STOMPClient:  bin/Book.o bin/connectionHandler.o bin/StompClient.o

bin/Book.o: src/Book.cpp
	g++ -pthread $(CFLAGS) -o bin/Book.o src/Book.cpp

bin/connectionHandler.o: src/connectionHandler.cpp
	g++ -pthread $(CFLAGS) -o bin/connectionHandler.o src/connectionHandler.cpp

bin/StompClient.o: src/StompClient.cpp
	g++ -pthread $(CFLAGS) -o bin/StompClient.o src/StompClient.cpp

.PHONY: clean
clean:
	rm -f bin/*
