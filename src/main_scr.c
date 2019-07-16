/* Main game screen */
#include <ncurses.h>
#include <string.h>
#include "initialization.h"

/* DEFINITIONS */
#define HUMAN_MODE 0
#define COMPU_MODE 1

#define NO_BOARDS 3

// Game boards -> if element = 0 -> empty space, 1 -> X
int boards[NO_BOARDS][3][3];

extern int which_mode;

extern WINDOW *main_win;
extern WINDOW *boards_win[NO_BOARDS];
extern WINDOW *menu_win;
extern WINDOW *error_win;
extern WINDOW *status_win;
extern WINDOW *endgame_win;

/* FUNCTIONS */
void game_mode();

void human_mode();
void compu_mode();

void fill_boards();

void print_boards(int x, int y);
void print_board(int board[3][3], WINDOW *board_win);
void print_menu(int which);
void print_status(int turn);
int print_endgame(int who_won);
void print_error(int error_num);

// Start choosen mode
void game_mode()
{
    // Clear main window & redraw borders
    wclear(main_win);
    box(main_win, 0, 0);
    wrefresh(main_win);

    // Fill game boards
    fill_boards();

    if (which_mode == HUMAN_MODE)
    {
        human_mode();
    }
    else if (which_mode == COMPU_MODE)
    {
        compu_mode();
    }
}

// Two players mode
void human_mode()
{
    // ...
}

// Computer mode
void compu_mode()
{
    // ...
}

// Fill boards with 0
void fill_boards()
{
    for (int i = 0; i < NO_BOARDS; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                boards[i][j][k] = 0;
            }
        }
    }
}

/* Printing functions */

// Print game boards
// if 1 -> X, 0 -> empty space
void print_boards(int x, int y)
{
    // Print boards without highlighting
    for (int i = 0; i < NO_BOARDS; i++)
    {
        print_board(boards[i], boards_win[i]);
    }

    // Find which board has highlighted element
    int which_board;
    if (x > 5)
    {
        which_board = 2;
    }
    else if (x > 2)
    {
        which_board = 1;
    }
    else
    {
        which_board = 0;
    }

    x %= 3;

    // Calculate printing position
    int where_x, where_y;

    where_y = 1 + (2 * y);
    where_x = 4 * x;

    // Find highlighted element character -> X if 1, empty space if 0
    char highlighted;

    highlighted = (boards[which_board][y][x]) ? 'X' : ' ';

    // Print highlighted element
    wattron(boards_win[which_board], A_BOLD | A_REVERSE);

    mvwprintw(boards_win[which_board], where_y, where_x, "/ %c /", highlighted);

    wattroff(boards_win[which_board], A_BOLD | A_REVERSE);

    wrefresh(boards_win[0]);
    wrefresh(boards_win[1]);
    wrefresh(boards_win[2]);
}

// Print a single board inside a given window -> no highlighting
void print_board(int board[3][3], WINDOW *board_win)
{
    // Grid
    char *grid[] = {" --- --- --- ",
                    "|   |   |   |",
                    " --- --- --- ",
                    "|   |   |   |",
                    " --- --- --- ",
                    "|   |   |   |",
                    " --- --- --- "};

    int grid_width, grid_height;
    grid_height = 7;
    grid_width = 13;

    // Print grid
    for (int i = 0; i < grid_height; i++)
    {
        mvwprintw(board_win, i, 0, "%s", grid[i]);
    }

    // Print elements
    int x, y;
    x = y = 0;

    for (int i = 1; i < grid_height; i += 2)
    {
        x = 0;

        for (int j = 2; j < grid_width; j += 4)
        {
            // Print character 
            switch (board[y][x])
            {
                case 0:
                    mvwaddch(board_win, i, j, ' ');
                    break;
                case 1:
                    mvwaddch(board_win, i, j, 'X');
                    break;
            }

            x++;
        }

        y++;
    }
}

// Print choices menu -> with highlighting, choices start at 0
void print_menu(int which)
{
    char *tag = " MENU:";
    char *menu[] = {" - Restart",
                    " - Continue",
                    " - Undo last move",
                    " - Redo last move",
                    " - Save game",
                    " - Quit"};

    char *highlighted_menu[] = {"\t -> Restart",
                                "\t -> Continue",
                                "\t -> Undo last move",
                                "\t -> Redo last move",
                                "\t -> Save game",
                                "\t -> Quit"};
    
    // Print borders & tag
    box(menu_win, 0, 0);
    mvwprintw(menu_win, 0, 1, "%s", tag);

    // Print menu with choice highlighted
    for (int i = 0, no_choices = 6; i < no_choices; i++)
    {
        // Highlighted choice
        if (i == which)
        {
            wattron(menu_win, A_BOLD | A_REVERSE);
            mvwprintw(menu_win, i + 2, 5, "%s", highlighted_menu[i]);
            wattroff(menu_win, A_BOLD | A_REVERSE);
        }
        else
        {
            mvwprintw(menu_win, i + 2, 5, "%s", menu[i]);
        }
    }

    wrefresh(menu_win);
}

// Print turn status
// If turn = 1 -> computer or player 1, turn = -1 -> otherwise
void print_status(int turn)
{
    char *tag = " STATUS:";
    char *status[] = {"Player 1 to play",
                      "Player 2 to play",
                      "Thinking ...",
                      "Your turn"};

    // Clear status window & print borders
    wclear(status_win);
    box(status_win, 0, 0);

    // Print tag
    mvwprintw(status_win, 0, 1, "%s", tag);

    // Printing position
    int x, y;
    x = 4;
    y = 1;


    // Print turn status
    if (which_mode == COMPU_MODE)
    {
        // Computer turn
        if (turn == 1)
        {
            mvwprintw(status_win, y, x, "%s", status[2]);
        }
        // User won
        else if (turn == -1)
        {
            mvwprintw(status_win, y, x, "%s", status[3]);
        }
    }
    else if (which_mode == HUMAN_MODE)
    {
        // Player 1 won
        if (turn == 1)
        {
            mvwprintw(status_win, y, x, "%s", status[0]);
        }
        // User won
        else if (turn == -1)
        {
            mvwprintw(status_win, y, x, "%s", status[1]);
        }
    }

    wrefresh(status_win);
}

// Print message & prompt after game ending
// If who_won = 1 -> computer or player 1, who_won = -1 -> otherwise
int print_endgame(int who_won)
{
    char *prompt     =  "Play again?";
    char *win_msg[]  = {"Congratulations",
                        "PLAYER ?",
                        "You won"};
    char *lose_msg[] = {"You lost",
                        "Better luck next time"};

    char *choices[] =             {"|        New  game        |", "|           Quit          |"};
    char *choices_highlighted[] = {"/        New  game        /", "/           Quit          /"};

    // Get window size & printing position
    int rows, cols, x1, x2;
    getmaxyx(endgame_win, rows, cols);

    // Print game ending message
    if (which_mode == COMPU_MODE)
    {
        // User won
        if (who_won == -1)
        {
            x1 = (cols - strlen(win_msg[0])) / 2;
            x2 = (cols - strlen(win_msg[2])) / 2;

            mvwprintw(endgame_win, 0, x1, "%s", win_msg[0]);
            mvwprintw(endgame_win, 1, x2, "%s", win_msg[2]);
        }
        // Computer won
        else if (who_won == 1)
        {
            x1 = (cols - strlen(lose_msg[0])) / 2;
            x2 = (cols - strlen(lose_msg[1])) / 2;

            mvwprintw(endgame_win, 0, x1, "%s", lose_msg[0]);
            mvwprintw(endgame_win, 1, x2, "%s", lose_msg[1]);
        }
    }
    else if (which_mode == HUMAN_MODE)
    {
        // Find winner 
        char *winner = (who_won == 1) ? "PLAYER 1" : "PLAYER 2";

        x1 = (cols - strlen(win_msg[0])) / 2;
        x2 = (cols - strlen(win_msg[1])) / 2;

        mvwprintw(endgame_win, 0, x1, "%s", win_msg[0]);
        mvwprintw(endgame_win, 1, x2, "%s", winner);
    }

    // Print initial state of options
    print_options(endgame_win, prompt, choices_highlighted, choices, 0);
    wrefresh(endgame_win);

    // Take user choice
    int ch, which;
    which = 0;
    while ((ch = getch()) != 'q')
    {
        // Check if user made a choice
        if (navigate(ch, &which))
        {
            return which - 1;
        }

        // Print choices with highlighting
        print_options(endgame_win, prompt, choices_highlighted, choices, which);
    }
}

// Print error messages
void print_error(int error_num)
{
    // Error messages
    char *error_msgs[] = {"Error: invalid move", "Error: no choice made", "Error: invalid key"};

    // Get window size & printing position
    int rows, cols, y, x;
    getmaxyx(error_win, rows, cols);

    x = (cols - strlen(error_msgs[error_num])) / 2;
    y = 1;

    // Clear error window
    wclear(error_win);

    // Print error message
    mvwprintw(error_win, y, x, "%s", error_msgs[error_num]);

    wrefresh(error_win);
}