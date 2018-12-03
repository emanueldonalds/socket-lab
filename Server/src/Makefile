CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-pthread -std=c++11 -D_DEFAULT_SOURCE -D_unix_ -g  -Wall -fpermissive -I../include

ODIR=src
SRCS=main.cpp Transaction.cpp UserPosition.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: server

server: $(OBJS)
	$(CXX) $(CPPFLAGS) -o server $(OBJS)

server.o: main.cpp

transaction.o: Transaction.cpp

userPosition.o: UserPosition.cpp

clean:
	$(RM) $(OBJS) server


