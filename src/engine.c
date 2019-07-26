/* Engine used in computer mode */
#include <ncurses.h>
#include <string.h>

/* Structure to hold boards & their corresponding values */
typedef struct boardValue
{
    int board[3][3];
    int value[2];
}boardValue;

/* DEFINITIONS */
#define NO_BOARDS    3
#define NO_ROTATIONS 8

// Board values
#define A 2
#define B 3
#define C 4
#define D 5

// Number of configurations possible for each number of Xs
#define ZERO_X  1
#define ONE_X   3 
#define TWO_X   8 
#define THREE_X 13
#define FOUR_X  15 
#define FIVE_X  5
#define SIX_X   1

extern int boards[NO_BOARDS][3][3];
extern int dead_boards[NO_BOARDS];

/* BOARDS CONFIGURATIONS */
// All possible configs (except dead boards)
// Boards with the same no. of Xs are grouped into arrays
const boardValue X0[ZERO_X] = {   {{{0, 0, 0},                     // Configuration 1
                                    {0, 0, 0},
                                    {0, 0, 0}}, {C, 0}}   };

const boardValue X1[ONE_X] =  {   {{{1, 0, 0},                     // Configuration 1
                                    {0, 0, 0},
                                    {0, 0, 0}}, {1, 0}},

                                  {{{0, 1, 0},                     // Configuration 2
                                    {0, 0, 0},
                                    {0, 0, 0}}, {1, 0}},

                                  {{{0, 0, 0},                     // Configuration 3
                                    {0, 1, 0},
                                    {0, 0, 0}}, {C, C}}   };

const boardValue X2[TWO_X] = {    {{{1, 1, 0},                     // Configuration 1
                                    {0, 0, 0},
                                    {0, 0, 0}}, {A, D}},

                                  {{{1, 0, 1},                     // Configuration 2
                                    {0, 0, 0},
                                    {0, 0, 0}}, {B, 0}},

                                  {{{1, 0, 0},                     // Configuration 3
                                    {0, 1, 0},
                                    {0, 0, 0}}, {B, 0}},

                                  {{{1, 0, 0},                     // Configuration 4
                                    {0, 0, 1},
                                    {0, 0, 0}}, {B, 0}},

                                  {{{1, 0, 0},                     // Configuration 5
                                    {0, 0, 0},
                                    {0, 0, 1}}, {A, 0}},

                                  {{{0, 1, 0},                     // Configuration 6
                                    {1, 0, 0},
                                    {0, 0, 0}}, {A, 0}},

                                  {{{0, 1, 0},                     // Configuration 7
                                    {0, 1, 0},
                                    {0, 0, 0}}, {B, 0}},

                                  {{{0, 1, 0},                     // Configuration 8
                                    {0, 0, 0},
                                    {0, 1, 0}}, {A, 0}}   };

const boardValue X3[THREE_X] = {  {{{1, 1, 0},                     // Configuration 1
                                    {1, 0, 0},
                                    {0, 0, 0}}, {B, 0}},

                                  {{{1, 1, 0},                     // Configuration 2
                                    {0, 1, 0},
                                    {0, 0, 0}}, {A, B}},

                                  {{{1, 1, 0},                     // Configuration 3
                                    {0, 0, 1},
                                    {0, 0, 0}}, {D, 0}},

                                  {{{1, 1, 0},                     // Configuration 4
                                    {0, 0, 0},
                                    {1, 0, 0}}, {A, 0}},

                                  {{{1, 1, 0},                     // Configuration 5
                                    {0, 0, 0},
                                    {0, 1, 0}}, {D, 0}},

                                  {{{1, 1, 0},                     // Configuration 6
                                    {0, 0, 0},
                                    {0, 0, 1}}, {D, 0}},

                                  {{{1, 0, 1},                     // Configuration 7
                                    {0, 1, 0},
                                    {0, 0, 0}}, {A, 0}},

                                  {{{1, 0, 1},                     // Configuration 8
                                    {0, 0, 0},
                                    {1, 0, 0}}, {A, B}},

                                  {{{1, 0, 1},                     // Configuration 9
                                    {0, 0, 0},
                                    {0, 1, 0}}, {A, 0}},

                                  {{{1, 0, 0},                     // Configuration 10
                                    {0, 1, 1},
                                    {0, 0, 0}}, {A, 0}},

                                  {{{1, 0, 0},                     // Configuration 11
                                    {0, 0, 1},
                                    {0, 1, 0}}, {1, 0}},

                                  {{{0, 1, 0},                     // Configuration 12
                                    {1, 1, 0},
                                    {0, 0, 0}}, {A, B}},

                                  {{{0, 1, 0},                     // Configuration 13
                                    {1, 0, 1},
                                    {0, 0, 0}}, {B, 0}}   };

const boardValue X4[FOUR_X] = {   {{{1, 1, 0},                     // Configuration 1
                                    {1, 1, 0},
                                    {0, 0, 0}}, {A, 0}},

                                  {{{1, 1, 0},                     // Configuration 2
                                    {1, 0, 1},
                                    {0, 0, 0}}, {A, 0}},

                                  {{{1, 1, 0},                     // Configuration 3
                                    {1, 0, 0},
                                    {0, 0, 1}}, {A, 0}},

                                  {{{1, 1, 0},                     // Configuration 4
                                    {0, 1, 1},
                                    {0, 0, 0}}, {B, 0}},

                                  {{{1, 1, 0},                     // Configuration 5
                                    {0, 1, 0},
                                    {1, 0, 0}}, {B, 0}},

                                  {{{1, 1, 0},                     // Configuration 6
                                    {0, 0, 1},
                                    {1, 0, 0}}, {B, 0}},

                                  {{{1, 1, 0},                     // Configuration 7
                                    {0, 0, 1},
                                    {0, 1, 0}}, {A, B}},

                                  {{{1, 1, 0},                     // Configuration 8
                                    {0, 0, 1},
                                    {0, 0, 1}}, {A, B}},

                                  {{{1, 1, 0},                     // Configuration 9
                                    {0, 0, 0},
                                    {1, 1, 0}}, {B, 0}},

                                  {{{1, 1, 0},                     // Configuration 10
                                    {0, 0, 0},
                                    {1, 0, 1}}, {B, 0}},

                                  {{{1, 1, 0},                     // Configuration 11
                                    {0, 0, 0},
                                    {0, 1, 1}}, {A, 0}},

                                  {{{1, 0, 1},                     // Configuration 12
                                    {0, 1, 0},
                                    {0, 1, 0}}, {B, 0}},

                                  {{{1, 0, 1},                     // Configuration 13
                                    {0, 0, 0},
                                    {1, 0, 1}}, {A, 0}},

                                  {{{1, 0, 0},                     // Configuration 14
                                    {0, 1, 1},
                                    {0, 1, 0}}, {B, 0}},

                                  {{{0, 1, 0},                     // Configuration 15
                                    {1, 0, 1},
                                    {0, 1, 0}}, {A, 0}}   };

const boardValue X5[FIVE_X] = {   {{{1, 1, 0},                     // Configuration 1
                                    {1, 0, 1},
                                    {0, 1, 0}}, {B, 0}},
                                      
                                  {{{1, 1, 0},                     // Configuration 2
                                    {1, 0, 1},
                                    {0, 0, 1}}, {B, 0}},

                                  {{{1, 1, 0},                     // Configuration 3
                                    {0, 1, 1},
                                    {1, 0, 0}}, {A, 0}},

                                  {{{1, 1, 0},                     // Configuration 4
                                    {0, 0, 1},
                                    {1, 1, 0}}, {A, 0}},

                                  {{{1, 1, 0},                     // Configuration 5
                                    {0, 0, 1},
                                    {1, 0, 1}}, {A, 0}}   };

const boardValue X6[SIX_X] = {    {{{1, 1, 0},                     // Configuration 1
                                    {1, 0, 1},
                                    {0, 1, 1}}, {A, 0}}   };

/* FUNCTIONS */
int compare();
int compare_boards(int board1[3][3], int board2[3][3]);

void rotate_board(int board[3][3], int rotations[NO_ROTATIONS][3][3]);

// Compares board with a configuration
// Returns 1: if the same, 0: otherwise
int compare(int board[3][3], boardValue config)
{
    // Find all rotations of a configuration
    int rotations[NO_ROTATIONS][3][3];
    rotate_board(config.board, rotations);

    // Compare board with rotations
    for (int i = 0; i < NO_ROTATIONS; i++)
    {
        if (compare_boards(board, rotations[i]))
        {
            return 1;
        }
    }

    return 0;
}

// Compare two boards
int compare_boards(int board1[3][3], int board2[3][3])
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            // If there's a difference
            if (board1[i][j] != board2[i][j])
            {
                return 0;
            }
        }
    }

    // There's no difference
    return 1;
}

// Produce all (8) possible rotations of a board configuration
// Fills array with 8 boards
void rotate_board(int board[3][3], int rotations[NO_ROTATIONS][3][3])
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            rotations[0][i][j] = board[i][j];
            rotations[1][i][j] = board[i][2 - j];

            rotations[2][i][j] = board[j][i];
            rotations[3][i][j] = board[j][2 - i];

            rotations[4][i][j] = board[2 - i][j];
            rotations[5][i][j] = board[2 - i][2 - j];

            rotations[6][i][j] = board[2 - j][i];
            rotations[7][i][j] = board[2 - j][2 - i];
        }
    }
}
