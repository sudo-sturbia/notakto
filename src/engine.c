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
#define NO_BOARDS 3

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

// All boards configurations (except dead boards)
// Boards with the same no. of Xs are grouped
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
