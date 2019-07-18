/* Validate & play moves */
#include <ncurses.h>
#include <string.h>

/* DEFINITIONS */
#define NO_BOARDS 3

extern int boards[NO_BOARDS][3][3];
extern int dead_boards[NO_BOARDS];

/* FUNCTIONS */
void play_move(int x, int y);

int is_valid(int x, int y);
int is_finished();

void mark_boards();
int is_dead(int board[3][3]);

// Play move on boards
void play_move(int x, int y)
{
    // Find which board to play on
    int which_board;

    which_board = x / 3;
    x %= 3;

    boards[which_board][y][x] = 1;
}

// Check if a move is valid
int is_valid(int x, int y)
{
    // Find which board the move is played on
    int which_board;

    which_board = x / 3;
    x %= 3;

    return !dead_boards[which_board] && !boards[which_board][y][x]; 
}

// Check if game is finished
// Returns 1: finished, 0: not
int is_finished()
{
    // Mark dead boards
    mark_boards();

    for (int i = 0; i < 3; i++)
    {
        if (!dead_boards[i])
        {
            return 0;
        }
    }

    return 1 ;
}

// Mark dead boards
void mark_boards()
{
    for (int i = 0; i < NO_BOARDS; i++)
    {
        if (!dead_boards[i])
        {
            dead_boards[i] = is_dead(boards[i]);
        }
    }
}

// Check if a board is dead
int is_dead(int board[3][3])
{
    // Check vertical & horizontal
    int v_sum, h_sum;

    for (int i = 0; i < 3; i++)
    {
        v_sum = h_sum = 0;

        for (int j = 0; j < 3; j++)
        {
            v_sum += board[j][i];
            h_sum += board[i][j];
        }

        // Check for a straight line
        if (v_sum == 3 || h_sum == 3)
        {
            return 1;
        }
    }

    // Check diagonal
    int d_sum1, d_sum2;
    d_sum1 = d_sum2 = 0;

    for (int i = 0; i < 3; i++)
    {
        d_sum1 += board[i][i];
        d_sum2 += board[2 - i][i];
    }

    // Check for a straight line
    if (d_sum1 == 3 || d_sum2 == 3)
    {
        return 1;
    }

    return 0;
}
