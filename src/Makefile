CC=g++
CFLAGS=-g -Wall 

mine: main.o curses.h
	$(CC) $(CFLAGS) main.cpp -o minesweeper -lncurses

clean:
	rm *.o minesweeper