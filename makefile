CC = gcc
CFLAGS = -g3 -Wall
SOURCE = assembler.c
EX = assembler

robson: FORCE
	$(CC) $(CFLAGS) $(SOURCE) -o $(EX) $(LINKS)
valgrind:
	valgrind ./$(EX)
FORCE:
