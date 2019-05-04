#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ncurses.h"

#define MAX_ROWS 4
#define MAX_COLS 4

int x, y;

void print_number(int value){
    printw("| ");
    while(value > 0){
        int digit = value%10;
        addch(digit + '0' | A_BOLD | A_UNDERLINE);
        value /= 10;
    }
    printw(" |");
}

void drawBoard(int board[][MAX_COLS], int num_rows,int num_cols){
    move(1, 0);
    board[2][3] = 52;
    for (int i=0; i<num_rows; i++){
        for (int j=0; j<num_cols; j++){
            print_number(board[i][j]);
        }
        printw("\n");
    }

    move(y-1, 0);
    printw("Input number: ");
}

int main() {
    initscr();
    wrefresh(stdscr);

    getmaxyx(stdscr, y, x);

    move(0, x/2-strlen("MINESWEEPER")/2);
    printw("MINESWEEPER");

    int board[MAX_ROWS][MAX_COLS] = {{0}};

    drawBoard(board, MAX_ROWS, MAX_COLS);

    getch();

    endwin();
}