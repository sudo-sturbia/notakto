/* Curses initialization & game start */
#include <ncurses.h>
#include "main_scr.h"

int main(void)
{
    // Start curses mode
    initscr();
    refresh();
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
