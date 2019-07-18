#ifndef MOVES_H_INCLUDED
#define MOVES_H_INCLUDED

/* FUNCTIONS */
void play_move(int x, int y);

int is_valid(int x, int y);
int is_finished();

void mark_boards();
int is_dead(int board[3][3]);

#endif
