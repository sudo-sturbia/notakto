// Initialize curses & start game
#include <ncurses.h>
#include "initialization.h"

int main(void)
{
    // Start curses mode
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);

    // Initialize game
    init_game();

    // End curses mode
    endwin();

    return 0;
}