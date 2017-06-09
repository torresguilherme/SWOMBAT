CC = gcc
CFLAGS = -g3 -Wall
SOURCE = mont.c func.c
EX = mont

all: FORCE
	$(CC) $(CFLAGS) $(SOURCE) -o $(EX)
valgrind:
	valgrind ./$(EX)
FORCE:

