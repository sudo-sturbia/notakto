// Windows used in game
#include <ncurses.h>
#include <string.h>

/* DEFINITIONS */


/* WINDOWS */
WINDOW *main_win;
WINDOW *boards_win[3];
WINDOW *logo_win;
WINDOW *instructions_win;
WINDOW *menu_win;
WINDOW *error_win;
WINDOW *status_win;

/* FUNCTIONS */
void create_windows();
void clear_windows();
void destroy_windows();

void create_windows()
{
    // Get screen size
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    // Variables to represent printing position & window size
    int x, y, height, width;

    // Create logo window
    height = 9;
    width = 64;
    y = rows - 9;
    x = 0;

    logo_win = newwin(height, width, y, x);

    // Create instructions window
    height = 9;
    width = cols - 65;
    y = rows - 9;
    x = 65;

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
    width = cols;
    y = rows - 9 - 3;
    x = 0;

    error_win = newwin(height, width, y, x);

    // Create status (turn) window
    height = 4;
    width = 24;
    y = 1;
    x = 1;

    status_win = newwin(height, width, y, x);
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