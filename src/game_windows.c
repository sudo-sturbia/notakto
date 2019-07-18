/* Windows used in game */
#include <ncurses.h>
#include <string.h>

/* DEFINITIONS */
#define LOGO_WIDTH 62
#define LOGO_HEIGHT 5

#define NO_BOARDS 3

/* WINDOWS */
WINDOW *main_win;
WINDOW *boards_win[NO_BOARDS];
WINDOW *logo_win;
WINDOW *instructions_win;
WINDOW *menu_win;
WINDOW *error_win;
WINDOW *status_win;
WINDOW *endgame_win;

/* FUNCTIONS */
void create_windows();
void clear_windows();
void destroy_windows();

void print_logo();
void print_instructions();

// Create windows used in game
void create_windows()
{
    // Get screen size
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    // Variables to represent printing position & window size
    int x, y, height, width;

    // Create logo window
    height = 9;
    width = 65;
    y = rows - 9;
    x = 0;

    logo_win = newwin(height, width, y, x);

    // Create instructions window
    height = 9;
    width = cols - 66;
    y = rows - 9;
    x = 66;

    instructions_win = newwin(height, width, y, x);

    // Create main game window
    height = rows - 9;
    width = cols;
    y = 0;
    x = 0;

    main_win = newwin(height, width, y, x);

    // Create boards windows -> inside main window
    height = 7;
    width = 13;
    y = (rows - height - 9) / 2;
    x = (cols - (3 * width + 2 * 8)) / 2;

    boards_win[0] = newwin(height, width, y, x);
    boards_win[1] = newwin(height, width, y, x + width * 1 + 8 * 1);
    boards_win[2] = newwin(height, width, y, x + width * 2 + 8 * 2);

    // Create menu window -> inside main window
    height = 10;
    width = 50;
    y = (rows - height - 9) / 2;
    x = (cols - width) / 2;

    menu_win = newwin(height, width, y, x);

    // Create error window -> inside main window
    height = 3;
    width = cols / 2;
    y = rows - 9 - 4;
    x = cols / 4;

    error_win = newwin(height, width, y, x);

    // Create status (turn) window
    height = 3;
    width = 24;
    y = rows - 9 - 4;
    x = 2;

    status_win = newwin(height, width, y, x);

    // Create game ending window
    height = 10;
    width = 60;
    y = (rows - height - 9) / 2;
    x = (cols - width) / 2;

    endgame_win = newwin(height, width, y, x);
}

// Clear all windows & remove borders
void clear_windows()
{
    // Clear all windows
    wclear(boards_win[0]);
    wclear(boards_win[1]);
    wclear(boards_win[2]);
    wclear(main_win);
    wclear(logo_win);
    wclear(instructions_win);
    wclear(menu_win);
    wclear(error_win);
    wclear(status_win);

    // Remove borders
    wborder(main_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wborder(logo_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wborder(menu_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wborder(instructions_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');

    // Refresh windows
    wrefresh(boards_win[0]);
    wrefresh(boards_win[1]);
    wrefresh(boards_win[2]);
    wrefresh(main_win);
    wrefresh(logo_win);
    wrefresh(instructions_win);
    wrefresh(menu_win);
    wrefresh(error_win);
    wrefresh(status_win);
}

// Deletes all windows
void destroy_windows()
{
    delwin(boards_win[0]);
    delwin(boards_win[1]);
    delwin(boards_win[2]);
    delwin(main_win);
    delwin(logo_win);
    delwin(instructions_win);
    delwin(menu_win);
    delwin(error_win);
    delwin(status_win);
}

/* Printing functions for static windows */
void print_logo()
{
    // Logo & author
    // Logo is represented using an integer array where -> 0: ' ', 1: block, 2: |, 4: -
    char *author = "@sudo-sturbia";
    int logo_arr[LOGO_HEIGHT][LOGO_WIDTH] = {{1, 1, 1, 2, 0, 0, 0, 1, 1, 2, 0, 1, 1, 1, 1, 1, 1, 2, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 1, 1,
                                              1, 1, 1, 2, 0, 1, 1, 2, 0, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 1, 1, 1, 1, 1, 1, 2, 0},
                                             {1, 1, 1, 1, 2, 0, 0, 1, 1, 2, 1, 1, 2, 4, 4, 4, 1, 1, 2, 0, 4, 4, 1, 1, 2, 4, 4, 0, 1, 1, 2,
                                              4, 4, 1, 1, 2, 1, 1, 2, 0, 1, 1, 2, 0, 0, 4, 4, 1, 1, 4, 4, 4, 0, 1, 1, 2, 4, 4, 4, 1, 1, 2},
                                             {1, 1, 2, 1, 1, 2, 0, 1, 1, 2, 1, 1, 2, 0, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 1,
                                              1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 2},
                                             {1, 1, 2, 0, 1, 1, 2, 1, 1, 2, 1, 1, 2, 0, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 2,
                                              4, 4, 1, 1, 2, 1, 1, 2, 4, 1, 1, 2, 0, 0, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 2},
                                             {1, 1, 2, 0, 0, 1, 1, 1, 1, 2, 0, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 2,
                                              0, 0, 1, 1, 2, 1, 1, 2, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 2, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 0}
                                             };

    // Logo window size
    int win_rows, win_cols;
    win_rows = 9;
    win_cols = 65;

    // Print borders
    box(logo_win, 0, 0);

    // Print logo
    for (int i = 0; i < LOGO_HEIGHT; i++)
    {
        for (int j = 0; j < LOGO_WIDTH; j++)
        {
            switch(logo_arr[i][j])
            {
                case 0:
                    mvwaddch(logo_win, i + 2, j + 2, ' ');
                    break;
                case 1:
                    mvwaddch(logo_win, i + 2, j + 2, ' ' | A_REVERSE);
                    break;
                case 2:
                    mvwaddch(logo_win, i + 2, j + 2, '|');
                    break;
                case 4:
                    mvwaddch(logo_win, i + 2, j + 2, '-');
                    break;
            }
        }
    }

    // Print author
    int y, x;
    y = win_rows - 2;
    x = win_cols - strlen(author) - 5;

    mvwprintw(logo_win, y, x, "%s", author);

    wrefresh(logo_win);
}

// Print game instructions 
void print_instructions()
{
    char *instructions[] = {" INSTRUCTIONS:",
                            "- To navigate use:  ^",
                            "                    k",
                            "               < h     l >",
                            "                    j",
                            "                    v",
                            "- To use menu press ESC",
                            "- To quit press q"};

    // Get window size & printing position
    int win_rows, win_cols, x;
    getmaxyx(instructions_win, win_rows, win_cols);

    x = win_cols / 8;
    x = (x > 0) ? x : 1;           // x must be higher than 0

    // Print borders
    box(instructions_win, 0, 0);

    // Print window tag
    mvwprintw(instructions_win, 0, 1, "%s", instructions[0]);

    // Print instructions
    for (int i = 1; i <= 7; i++)
    {
        mvwprintw(instructions_win, i, x, "%s", instructions[i]);
    }

    wrefresh(instructions_win);
}
