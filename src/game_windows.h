#ifndef GAME_WINDOWS_H_INCLUDED
#define GAME_WINDOWS_H_INCLUDED

/* FUNCTIONS */
int create_windows();
void clear_windows();
void destroy_windows();

void exit_game(int code);

void adjust_windows();
void resize_or_quit(int ch);

void print_logo();
void print_instructions();

void print_side_menu(int which_win, int is_used);
void print_boards(int x, int y);
void print_board(int board[3][3], WINDOW *board_win);
void print_menu(int which);
void print_status(int turn);
void print_stats(int engine_games[2], int two_user_games[2]);
void print_end_msg(int who_won);
void print_error(int error_num, int which_win);

#endif
