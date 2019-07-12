// Initialize curses & start game
#include <ncurses.h>
#include "game_windows.h"

int main(void)
{
    // Start curses mode
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);

    // Create windows used in game
    create_windows();

    print_logo();
    getchar();

    // End curses mode
    endwin();

    return 0;
}