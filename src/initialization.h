#ifndef INITIALIZATION_H_INCLUDED
#define INITIALIZATION_H_INCLUDED 

/* FUNCTIONS */
void init_game();
void initial_msg();

int choose_mode();
int playing_order();

int navigate(int ch, int *which_pr);

void print_options(WINDOW *which_win, char *prompt, char *highlighted[], char *not_highlighted[], int which);

#endif
