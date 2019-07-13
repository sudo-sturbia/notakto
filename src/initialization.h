#ifndef INITIALIZATION_H
#define INITIALIZATION_H

/* FUNCTIONS */
void init_game();
void initial_msg();

int choose_mode();
int playing_order();

int navigate(int ch, int *which_pr);

void print_options(char *prompt, char *highlighted[], char *not_highlighted[], int which);

#endif