/* Main game screen */
#include <ncurses.h>
#include <string.h>

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
extern WINDOW *status_win;
extern WINDOW *error_win;

/* FUNCTIONS */
void game_mode();

void human_mode();
void compu_mode();

void fill_boards();

void print_boards(int x, int y);
void print_board(int board[3][3], WINDOW *board_win);
void print_menu(int which);
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