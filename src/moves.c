/* Validate moves, save/load & undo/redo */
#include <ncurses.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> 

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
extern int which_mode;
extern int turn;
extern WINDOW *main_win;

/* FUNCTIONS */
void play_move(int x, int y);

int is_valid(int x, int y);
int is_finished();

void mark_boards();
int is_dead(int board[3][3]);

void save_game();
char *file_name_prompt();
int write_game_data(char *file_name);
void write_node(node *node_to_write, FILE *game_file);

int load_game();
int check_game_data(FILE *game_file, int *number_of_nodes);
void read_game_data(FILE *game_file, int number_of_nodes);
void read_undo_stack(FILE *game_file, int number_of_nodes);

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
        dead_boards[i] = is_dead(boards[i]);
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

/* SAVE & LOAD GAMES */

// Save current game
void save_game()
{
    // Prompt user for file name
    char *file_name = file_name_prompt();

    // Save game data
    if (strlen(file_name) == 0 || !write_game_data(file_name))    // No file name or Not saved correctly
    {
        print_error(8, 1);
        resize_or_quit(getch());

        return;
    }

    free(file_name);

    wclear(main_win);
    box(main_win, 0, 0);

    // Print success message
    char *success = "Game succefully saved, press any key to continue";

    int ROWS, COLS;
    getmaxyx(main_win, ROWS, COLS);

    mvwprintw(main_win, ROWS / 2, (COLS - strlen(success)) / 2, "%s", success);
    wrefresh(main_win);

    resize_or_quit(getch());
}

// Prompt user for a file name
// Returns name of file to open or an empty string
char *file_name_prompt()
{
    const int MAX_INPUT_SIZE = 40;

    int ROWS, COLS;
    getmaxyx(main_win, ROWS, COLS);

    // Print warning & prompt
    char *prompt[]  = {"Enter file name - Alphanumeric characters only",
                       "Press ENTER to proceed"};

    wclear(main_win);
    box(main_win, 0, 0);

    mvwprintw(main_win, (ROWS - 5) / 2, (COLS - strlen(prompt[0]))/ 2, "%s", prompt[0]);
    mvwprintw(main_win, (ROWS - 7) / 2, (COLS - strlen(prompt[1]))/ 2, "%s", prompt[1]);

    wrefresh(main_win);

    // Create a box for input
    WINDOW *inner_box;
    WINDOW *outer_box;

    outer_box = newwin(3, (MAX_INPUT_SIZE + 2), (ROWS - 3) / 2, (COLS - (MAX_INPUT_SIZE + 2)) / 2);
    inner_box = newwin(1, MAX_INPUT_SIZE, (ROWS - 1) / 2, (COLS - MAX_INPUT_SIZE) / 2);

    box(outer_box, 0, 0);
    wrefresh(outer_box);
    wrefresh(inner_box);

    // Take input
    curs_set(1);
    keypad(inner_box, FALSE);

    int ch;
    int char_counter = 0;
    char input_str[MAX_INPUT_SIZE];

    while ((ch = wgetch(inner_box)) != 10)
    {
        wmove(inner_box, 0, char_counter);

        // If terminal is resized -> abort
        if (ch == KEY_RESIZE)
        {
            delwin(inner_box);
            delwin(outer_box);
            curs_set(0);
            adjust_windows();
            return "";
        }

        // Delete character
        if ((ch == KEY_BACKSPACE || ch == KEY_DC || ch == 127) && (char_counter > 0))
        {
            char_counter--;
        }
        // Add alphanumeric character
        else if ((isalpha(ch) || isdigit(ch)) && char_counter < MAX_INPUT_SIZE)
        {
            input_str[char_counter++] = ch;
        }

        // Print character array until cursor position
        wclear(inner_box);
        for (int i = 0; i < char_counter; i++)
        {
            mvwaddch(inner_box, 0, i, input_str[i]);
            wrefresh(inner_box);
        }
    }

    input_str[MAX_INPUT_SIZE] = '\0';

    if (char_counter == 0)
    {
        delwin(inner_box);
        delwin(outer_box);
        curs_set(0);
        return "";
    }

    // Create a new string with the exact size to return
    char *file_name = (char *) malloc(char_counter *sizeof(char));
    for (int i = 0; i < char_counter; i++)
    {
        file_name[i] = input_str[i];
    }

    file_name[char_counter] = '\0';

    delwin(inner_box);
    delwin(outer_box);
    curs_set(0);

    return file_name; 
}

// Write game data to a file
// Returns 1 : if correctly saved, 0 : otherwise
int write_game_data(char *file_name)
{
    // Create a temporary stack reference
    node *temp_stack = undo_stack;

    // Append directory name to file name
    char *dir_name = "saved-games/";
    char *file = (char *) malloc((strlen(dir_name) + strlen(file_name)) * sizeof(char));    

    strcat(strcpy(file, dir_name), file_name);

    // Create a directory to save games to (if non-existant)
    struct stat st;
    if (stat("saved-games", &st) == -1)   // If directory doesn't exist
    {
        mkdir("saved-games", 0755);
    }

    // Open file to write
    FILE *game_file;
    game_file = fopen(file, "w");

    if (game_file == NULL)
    {
        return 0;
    }

    // Write game data to file
    // Game mode
    fputc(which_mode, game_file);

    // Turn
    fputc(((turn == -1) ? 0 : turn), game_file);

    // Dead boards array
    for (int i = 0; i < NO_BOARDS; i++)
    {
        fputc(dead_boards[i], game_file);
    }

    // Game boards
    for (int i = 0; i < NO_BOARDS; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                fputc(boards[i][j][k], game_file);
            }
        }
    }

    // Undo stack nodes
    while (temp_stack != NULL)
    {
        write_node(temp_stack, game_file);
        temp_stack = temp_stack -> next;
    }

    free(file);
    fclose(game_file);

    return 1;
}

// Write node to a file
void write_node(node *node_to_write, FILE *game_file)
{
    for (int i = 0; i < NO_BOARDS; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                fputc(node_to_write -> value[i][j][k], game_file);
            }
        }
    }
}

// Load previously saved game
// Returns 1: loaded correctly, 0: otherwise
int load_game()
{
    // Prompt user for file name
    char *file_name = file_name_prompt();

    if (strlen(file_name) == 0)
    {
        print_error(10, 1);
        resize_or_quit(getch());

        return 0;
    }

    // Append directory name to file name
    char *dir_name = "saved-games/";
    char *file = (char *) malloc((strlen(dir_name) + strlen(file_name)) * sizeof(char));

    strcat(strcpy(file, dir_name), file_name);

    // Open file
    FILE *game_file;
    game_file = fopen(file, "r");
    if (game_file == NULL)
    {
        print_error(9, 1);
        resize_or_quit(getch());

        return 0;
    }

    // Check game data
    int number_of_nodes;
    if (check_game_data(game_file, &number_of_nodes))
    {
        // Read game data
        read_game_data(game_file, number_of_nodes - 1);
    }
    else
    {
        print_error(10, 1);
        resize_or_quit(getch());

        return 0;
    }

    free(file);
    free(file_name);
    fclose(game_file);

    return 1;
}

// Check game file data
// Return 1: if data is correct, 0: if not
int check_game_data(FILE *game_file, int *number_of_nodes)
{
    // Count characters & check their value
    int ch;
    int counter = 0;

    while ((ch = fgetc(game_file)) != EOF)
    {
        counter++;

        if (ch != 1 && ch != 0)
        {
            return 0;
        }
    }

    if (!((counter - 5) % 27))
    {
        *number_of_nodes = ((counter - 5) / 27) - 1;
        return 1;
    }
    else
    {
        return 0;
    }
}

// Read game data
void read_game_data(FILE *game_file, int number_of_nodes)
{
    // Read game data
    fseek(game_file, 0, SEEK_SET);

    // Game mode
    which_mode = fgetc(game_file);

    // Turn
    turn = fgetc(game_file);
    turn = (turn == 0) ? -1 : turn;

    // Dead boards array
    for (int i = 0; i < NO_BOARDS; i++)
    {
        dead_boards[i] = fgetc(game_file);
    }

    // Game boards
    for (int i = 0; i < NO_BOARDS; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                boards[i][j][k] = fgetc(game_file); 
            }
        }
    }

    // Undo stack nodes
    if (number_of_nodes >= 0)
    {
        init_stacks();
        read_undo_stack(game_file, number_of_nodes);
    }
}

// Read undo stack nodes from a file
void read_undo_stack(FILE *game_file, int number_of_nodes)
{
    // Read node boards
    int node_boards[NO_BOARDS][3][3];

    for (int i = 0; i < NO_BOARDS; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                node_boards[i][j][k] = fgetc(game_file);
            }
        }
    }

    if (number_of_nodes)
    {
        read_undo_stack(game_file, --number_of_nodes);
    }

    // Push boards to undo stack
    undo_stack = push(undo_stack, node_boards);
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
        print_error(5, 0);
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
        print_error(6, 0);
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
