XMLLDFLAGS!=pkg-config --libs libxml-2.0
XMLLDFLAGS?=$(shell pkg-config --libs libxml-2.0)
XMLCFLAGS!=pkg-config --cflags libxml-2.0
XMLCFLAGS?=$(shell pkg-config --cflags libxml-2.0)

CXXFLAGS:=$(XMLCFLAGS) -static-libstdc++ -std=c++17 -Wall -g
LDLIBS:=$(XMLLDFLAGS) -lssl -lcrypto
PRJ=feedreader
CC=g++

all:
	$(CC) $(CXXFLAGS) feedreader.cpp feedreader.h connection.cpp connection.h local_error.cpp local_error.h \
	proc_response.cpp proc_response.h $(LDLIBS) -o feedreader

test:
	./test

tar:
	tar cf sietky.tar *.cpp *.h Makefile README.md

cleantar:
	rm -f *.tar

clean:
	rm -f *.o *.tmp $(PRJ)
