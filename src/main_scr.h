#ifndef MAIN_SCR_H_INCLUDED
#define MAIN_SCR_H_INCLUDED

/* FUNCTIONS */
void init_game();

int play_two_user(int loaded);
int play_compu(int loaded);

int get_user_move();

int navigate_boards(int ch, int *x_pr, int *y_pr, int *menu_choice);
int use_menu();
int use_side_menu(int which_win);

void fill_boards();

void initial_msg();

int new_or_load();
int choose_mode();
int playing_order();
int play_again(int who_won);

int navigate(int ch, int *which_pr);

void print_options(WINDOW *which_win, char *prompt, char *highlighted[], char *not_highlighted[], int which);

#endif
