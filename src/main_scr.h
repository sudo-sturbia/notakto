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

int play_again(int who_won);

#endif
