CC = gcc
CFLAGS = -g3 -Wall
SOURCE = assembler.c
EX = assembler

all: FORCE
	$(CC) $(CFLAGS) $(SOURCE) -o $(EX)
valgrind:
	valgrind ./$(EX)
FORCE:

