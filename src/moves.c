/* Validate moves & undo / redo */
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "game_windows.h"

/* DEFINITIONS */
#define NO_BOARDS 3

/* node -> for undo & redo stacks */
typedef struct node
{
    int value[NO_BOARDS][3][3];
    struct node *next;
}node;

node *undo_stack;
node *redo_stack;

extern int boards[NO_BOARDS][3][3];
extern int dead_boards[NO_BOARDS];

/* FUNCTIONS */
void play_move(int x, int y);

int is_valid(int x, int y);
int is_finished();

void mark_boards();
int is_dead(int board[3][3]);

node *create_node(int value[NO_BOARDS][3][3], node *next);

void init_stacks();

node *push(node *head, int value[NO_BOARDS][3][3]);
node *pop(node *head);
void replace_with_top(node *head);

void undo();
void redo();

void clear_stacks();
void clear_undo();
void clear_redo();

// Play move on boards
void play_move(int x, int y)
{
    // Find which board to play on
    int which_board;

    which_board = x / 3;
    x %= 3;

    // Push boards to undo stack & clear redo stack
    undo_stack = push(undo_stack, boards);
    clear_redo();

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

/* UNDO & REDO */

// Create node
node *create_node(int value[NO_BOARDS][3][3], node *next)
{
    node *temp = (node *) malloc(sizeof(node));
    
    memcpy(temp -> value, value, sizeof(int) * 3 * 3 * 3);
    temp -> next  = next;

    return temp;
}

// Initialize stacks
void init_stacks()
{
    undo_stack = NULL;
    redo_stack = NULL;
}

// Push boards to stack -> return new head
node *push(node *head, int value[NO_BOARDS][3][3])
{
    node *temp = create_node(value, head);
    return temp;
}

// Pop head of the stack -> return new head
node *pop(node *head)
{
    if (head != NULL)
    {
        head = head -> next;
    }

    return head;
}

// Replace game boards with the top of the stack 
void replace_with_top(node *head)
{
   for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                boards[i][j][k] = head -> value[i][j][k];
            }
        }
    }
}

// Undo last move
void undo()
{
    // Empty undo stack
    if (undo_stack == NULL)
    {
        print_error(5);
        return;
    }

    // Push boards to redo stack
    redo_stack = push(redo_stack, boards);

    // Return to last move in undo stack
    replace_with_top(undo_stack);
    undo_stack = pop(undo_stack);

    mark_boards();
}

// Redo last move
void redo()
{
    // Empty redo stack
    if (redo_stack == NULL)
    {
        print_error(6);
        return;
    }

    // Push boards to undo stack
    undo_stack = push(undo_stack, boards);

    // Return to last move in redo stack
    replace_with_top(redo_stack);
    redo_stack = pop(redo_stack);

    mark_boards();
}

// Clear stacks
void clear_stacks()
{
    clear_undo();
    clear_redo();
}

// Clear undo stack
void clear_undo()
{
    node *temp;

    // Delete all nodes
    while (undo_stack != NULL)
    {
        temp = undo_stack;
        undo_stack = undo_stack -> next;

        free(temp);
    }
}

// Clear redo stack
void clear_redo()
{
    node *temp;

    // Delete all nodes
    while (redo_stack != NULL)
    {
        temp = redo_stack;
        redo_stack = redo_stack -> next;

        free(temp);
    }
}
