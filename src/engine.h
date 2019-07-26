#ifndef ENGINE_H_INCLUDED 
#define ENGINE_H_INCLUDED

/* DEFINITIONS */
#define BOARD_VALUE  2
#define POS_VALUE    6

#define NO_BOARDS    3
#define NO_ROTATIONS 8

typedef struct boardValue boardValue;

/* FUNCTIONS */
void find_pos_value(int pos[NO_BOARDS][3][3], int is_dead[3], int pos_value[POS_VALUE]);
void find_board_value(int board[3][3], int is_dead, int value[BOARD_VALUE]);

int compare();
int compare_boards(int board1[3][3], int board2[3][3]);

void rotate_board(int board[3][3], int rotations[NO_ROTATIONS][3][3]);

#endif 
