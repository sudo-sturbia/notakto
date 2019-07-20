#ifndef MAIN_SCR_H_INCLUDED
#define MAIN_SCR_H_INCLUDED

/* FUNCTIONS */
void play_game();

int play_two_user();
int play_compu();

int navigate_side_menu(int which_win);
int navigate_boards(int ch, int *x_pr, int *y_pr, int *menu_choice);
int use_menu();

void fill_boards();

void print_side_menu(int which_win);
void print_boards(int x, int y);
void print_board(int board[3][3], WINDOW *board_win);
void print_menu(int which);
void print_status(int turn);
int print_endgame(int who_won);
void print_error(int error_num);

#endif
