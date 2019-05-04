#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <set>
#include <random>
#include <queue>

#include "ncurses.h"

#define MAX_ROWS 15
#define MAX_COLS 15
#define MAX_MESSAGE 1024
#define MAX_BOMBS 20



typedef struct Cell{
    int i, j;
} Cell;



using namespace std;

int x, y;


void clear_cells(int board[][MAX_COLS], int bombs[][MAX_COLS], int i, int j, int visited[][MAX_COLS]){
    queue<Cell> cells;

    Cell cell = {.i = i, .j = j};
    cells.push(cell);

    while(!cells.empty()){
        Cell current = cells.front(); cells.pop();
        visited[current.i][current.j] = 1;

        /*Visit neighbors*/
        for (int row=-1; row<2; row++){
            for (int col=-1; col<2; col++){
                /*If it's inside the limits*/
                if (current.i+row >= 0 && current.i+row <MAX_ROWS && current.j+col >= 0 && current.j+col < MAX_COLS){
                    /*If its a unselected cell or a bombless cell*/
                    if (board[current.i+row][current.j+col]!=1 && visited[current.i+row][current.j+col]!=1){
                        /*Mark this cell as selected (has no bomb)*/
                        board[current.i+row][current.j+col] = -1;
                        /*If it has a bomb distance, don't add it to the queue*/
                        if (bombs[current.i+row][current.j+col] != 0){
                            /*Add to the queue*/
                            Cell next = {.i = current.i+row, .j = current.j+col};
                            cells.push(next);
                        }
                    }
                }
            }
        }
    }
}


int search_cell(int board[][MAX_COLS], int i, int j){
    if (i >= 0 && i < MAX_ROWS && j >= 0 && j < MAX_COLS){
        return board[i][j]==1?1:0;
    }
    return 0;
}

void get_bomb_distances(int board[][MAX_COLS], int bombs[][MAX_COLS]){
    for (int i=0; i<MAX_ROWS; i++){
        for (int j=0; j<MAX_COLS; j++){
            if (board[i][j]<=0){
                bombs[i][j] = search_cell(board, i-1, j-1) + search_cell(board, i-1, j) +
                    search_cell(board, i-1, j+1) + search_cell(board, i, j-1) + 
                    search_cell(board, i, j+1) + search_cell(board, i+1, j-1) +
                    search_cell(board, i+1, j) + search_cell(board, i+1, j+1);
            }
        }
    }
}


void generate_bombs(int board[][MAX_COLS], int num_rows, int num_cols, int bombs){
    set<int> current;
    int dice_roll;
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1,MAX_ROWS*MAX_COLS);
    for (int i=0; i<bombs; i++){
        do{
            dice_roll = distribution(generator);
        } while(current.find(dice_roll) != current.end() 
                || board[(dice_roll-1)/MAX_ROWS][(dice_roll-1)%MAX_COLS]==-1);

        board[(dice_roll-1)/MAX_ROWS][(dice_roll-1)%MAX_COLS] = 1;
        current.insert(dice_roll);
    }
}



void print_log(vector<string> log, int n){
    vector<string>::reverse_iterator it = log.rbegin();
    for (int i=1; i<=n && it != log.rend(); i++){
        move(y-i-1, x/2); clrtoeol();
        printw("Entered: %s", (*it).c_str());
        it++;
    }

    if (log.size()>3){
        log.erase(log.begin());
    }
}





void drawBoard(int board[][MAX_COLS], int bombs[][MAX_COLS], int num_rows, int num_cols, bool ended){
    move(1, 0);

    start_color();
    init_pair(1, COLOR_RED, -1);
    init_pair(2, COLOR_BLUE, -1);

    int counter=1;
    for (int i=0; i<num_rows; i++){
        for (int j=0; j<num_cols; j++){
            printw("| ");
            if (board[i][j]>0 && ended){
                attron(COLOR_PAIR(1));
                printw("%*d ", 3, counter);
                attroff(COLOR_PAIR(1));
            }
            else if (board[i][j] < 0){
                if (bombs[i][j] > 0){
                    attron(COLOR_PAIR(2));
                    printw("%*d ", 3, bombs[i][j]);
                    attroff(COLOR_PAIR(2));
                }
                else printw("%*c ", 5, '\0');
            }
            else printw("%*d ", 3, counter);
            counter++;
        }
        printw("|");
        printw("\n");
    }
}





int main() {
    initscr();
    wrefresh(stdscr);
    use_default_colors();

    getmaxyx(stdscr, y, x);

    move(0, x/2-strlen("MINESWEEPER")/2);
    printw("MINESWEEPER");

    int board[MAX_ROWS][MAX_COLS] = {{0}};
    int bomb_distances[MAX_ROWS][MAX_COLS] = {{0}};
    int visited[MAX_ROWS][MAX_COLS] = {{0}};


    drawBoard(board, bomb_distances, MAX_ROWS, MAX_COLS, false);

    char temp[127];
    vector<string> log;

    bool first_move = true;

    while(true){
        move(y-1, 0);
        clrtoeol();
        printw("Input number: ");
        getstr(temp);

        if (strcmp(temp, "exit")==0){
            endwin();
            exit(0);
        }
        else if (atoi(temp)==0 || atoi(temp)<0 || atoi(temp)>MAX_ROWS*MAX_COLS){
            log.push_back("Invalid");
            print_log(log, 3);
            continue;
        }
        else if(board[((int)atoi(temp)-1)/MAX_ROWS][(atoi(temp)-1)%MAX_COLS]==1){
            log.push_back("Game ended");
            drawBoard(board, bomb_distances, MAX_ROWS, MAX_COLS, true);
            print_log(log, 3);
            break;
        }
        log.push_back(temp);

        board[((int)atoi(temp)-1)/MAX_ROWS][(atoi(temp)-1)%MAX_COLS] = -1;

        if (first_move){
            generate_bombs(board, MAX_ROWS, MAX_COLS, MAX_BOMBS);
            get_bomb_distances(board, bomb_distances);
            first_move = false;
        }

        clear_cells(board, bomb_distances, (atoi(temp)-1)/MAX_ROWS, (atoi(temp)-1)%MAX_COLS, visited);
        drawBoard(board, bomb_distances, MAX_ROWS, MAX_COLS, false);

        print_log(log, 3);
    }

    getch();
    endwin();
}