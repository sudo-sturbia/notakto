#ifndef ENGINE_H_INCLUDED 
#define ENGINE_H_INCLUDED

/* DEFINITIONS */
#define NO_BOARDS    3
#define NO_ROTATIONS 8

typedef struct boardValue boardValue;

/* FUNCTIONS */
int compare();
int compare_boards(int board1[3][3], int board2[3][3]);

void rotate_board(int board[3][3], int rotations[NO_ROTATIONS][3][3]);

#endif 
