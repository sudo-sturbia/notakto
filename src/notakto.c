// Initialize curses & start game
#include <ncurses.h>

int main(void)
{
    // Start curses mode
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);

    // ...

    // End curses mode
    endwin();

    return 0;
}