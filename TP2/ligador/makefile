CC = gcc
CFLAGS = -g3 -Wall
SOURCE = link.c flink.c
EX = link

all: FORCE
	$(CC) $(CFLAGS) $(SOURCE) -o $(EX)
valgrind:
	valgrind ./$(EX)
FORCE:

