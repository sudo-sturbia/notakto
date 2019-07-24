#ifndef MOVES_H_INCLUDED
#define MOVES_H_INCLUDED

typedef struct node node;

/* DEFINITIONS */
#define NO_BOARDS 3 

/* FUNCTIONS */
void play_move(int x, int y);

int is_valid(int x, int y);
int is_finished();

void mark_boards();
int is_dead(int board[3][3]);

void save_game();
int load_game();

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

#endif
