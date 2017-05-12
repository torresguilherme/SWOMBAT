CC = gcc
CFLAGS = -O2 -g3 -Wall
LINKS = -lm
SOURCE = assembler.c
EX = assembler

robson: FORCE
	$(CC) $(CFLAGS) $(SOURCE) -o $(EX) $(LINKS)
valgrind:
	valgrind ./$(EX)
FORCE:
