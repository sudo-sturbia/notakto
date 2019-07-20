#ifndef GAME_WINDOWS_H_INCLUDED
#define GAME_WINDOWS_H_INCLUDED

/* FUNCTIONS */
void create_windows();
void clear_windows();
void destroy_windows();

void print_logo();
void print_instructions();

void print_side_menu(int which_win);
void print_boards(int x, int y);
void print_board(int board[3][3], WINDOW *board_win);
void print_menu(int which);
void print_status(int turn);
int print_endgame(int who_won);
void print_error(int error_num);

#endif
