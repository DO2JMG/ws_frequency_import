CC      = gcc
CXX     = g++
CFLAGS  = -g -O3 -Wall  -std=c++0x -lcurl -pthread -DHAVE_LOG_H
LIBS    = -lpthread 
LDFLAGS = -g

OBJECTS = main.o 

all:		main

main:	$(OBJECTS)
		$(CXX) $(OBJECTS) $(CFLAGS) $(LIBS) -o fqimport

%.o: %.cpp
		$(CXX) $(CFLAGS) -c -o $@ $<

                                  