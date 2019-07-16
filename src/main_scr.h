#ifndef MAIN_SCR_H
#define MAIN_SCR_H

/* FUNCTIONS */
void game_mode();

void human_mode();
void compu_mode();

void fill_boards();

void print_boards(int x, int y);
void print_board(int board[3][3], WINDOW *board_win);
void print_menu(int which);
void print_status(int turn);
int print_endgame(int who_won);
void print_error(int error_num);

#endif