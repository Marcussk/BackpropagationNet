#
# bpnetwork makefile
# Author: Marek Beňo, xbenom01
#

NAME=bpnetwork

CC=g++
CFLAGS=-std=c++11 -Wall -Wextra
LFLAGS=
SRCFOLDER=src
SOURCEFILES=src/Arguments.cpp src/Network.cpp src/Neuron.cpp src/main.cpp
HEADERFILES=src/Arguments.h src/Layer.h src/Link.h src/Logger.h src/Network.h src/Neuron.h
DATAFILES=data/winequality-red-normalized.csv data/winequality-red-train.csv data/winequality-red-test.csv
DATAFILES+= data/winequality-white-normalized.csv data/winequality-white-test.csv data/winequality-white-train.csv
ALLFILES=$(SOURCEFILES) $(HEADERFILES) $(DATAFILES) doc.pdf configuration.cfg Makefile 3000-1.log 3000-2.log 3000-3.log

ARCHIVENAME=xbenom01

all: $(NAME)

$(NAME): $(SOURCEFILES)
	$(CC) $(CFLAGS) $(SOURCEFILES) -o $(NAME) $(LFLAGS)

run: $(NAME)
	./$(NAME) 1 15 0.7 3000 0 11 data/winequality-red-train.csv data/winequality-red-test.csv

archive:
	zip $(ARCHIVENAME).zip $(ALLFILES)

debug: CFLAGS+=-g
debug: $(NAME)
	ddd $(NAME)

clean:
	rm -f $(NAME)

clean-all: clean
	rm -f $(ARCHIVENAME).zip
	rm -f $(ARCHIVENAME).tar.gz

.PHONY: run archive debug clean clean-all
