// Initialization & game beginning choices
#include <ncurses.h>
#include <string.h>
#include "game_windows.h"

/* DEFINITIONS */
#define HUMAN_MODE 0
#define COMPU_MODE 1

#define NO_OPTIONS 2              // Number of game beginning choices

int which_mode;

extern WINDOW *main_win;
extern WINDOW *error_win;

/* FUNCTIONS */
void init_game();
int start();
void print_options(char *prompt, char *highlighted[], char *not_highlighted[], int choice_size, int which);

// Start game & take beginning choices
void init_game()
{
    // Create windows needed in game
    create_windows();
    refresh();

    // Display static windows
    print_logo();
    print_instructions();

    // Start game & get beginning choices
    // ...
    
    start();
}

// Prompt user to start game or display game guide
// Returns 1: start, 0: guide
int start()
{
    char *start_msg = "PRESS ANY KEY TO START";
    char *prompt = "Choose a playing mode";
    char *modes[] =             {"|       TWO PLAYERS       |", "|   PLAY vs THE MACHINE   |"};
    char *modes_highlighted[] = {"/       TWO PLAYERS       /", "/   PLAY vs THE MACHINE   /"};

    // Get screen size & printing position
    int rows, cols, x, y;
    getmaxyx(main_win, rows, cols);

    y = rows / 2;
    x = (cols - strlen(start_msg)) / 2;

    // Print start message
    box(main_win, 0, 0);
    mvwprintw(main_win, y, x, "%s", start_msg);

    wrefresh(main_win);
    getch();
    wclear(main_win);

    // Print initial state of choices
    int choice_size = 27;

    print_options(prompt, modes_highlighted, modes, choice_size, 0);
    wrefresh(main_win);

    // Take user choice
    int ch, which;
    which = 0;
    while ((ch = getch()) != 113)
    {
        // Clear main & error windows & reprint borders
        wclear(main_win);
        wclear(error_win);
        wrefresh(error_win);

        // Highlight choice
        switch(ch)
        {
            // Move left
            case KEY_LEFT:
            case 104:
                which--;
                // Check_borders
                if (which < 1 || which > 2)
                {
                    print_error(0);
                }
                which = 1;
                break;
            // Move right
            case KEY_RIGHT:
            case 108:
                which++;
                // Check borders
                if (which > 2 || which < 1)
                {
                    print_error(0);
                }
                which = 2;
                break;
            // Enter key -> made a choice
            case 10:
                if (!which)
                {
                    print_error(1);
                }
                else
                {
                    return which;
                }
            // Invalid key
            default:
                print_error(2);
        }

        // Print choices with highlighting
        box(main_win, 0, 0);
        print_options(prompt, modes_highlighted, modes, choice_size, which);

        // Re-draw error window
        redrawwin(error_win);
        wrefresh(error_win);
    }
}

// Print choice options -> highlight position
// If which : 1 -> highlight 1st, 2 -> highlight 2nd, 0 -> no highlighting
void print_options(char *prompt, char *highlighted[], char *not_highlighted[], int choice_size, int which)
{
    // Get window size &  printing position
    int rows, cols, y, x;
    getmaxyx(main_win, rows, cols);

    // Print prompt
    x = (cols - strlen(prompt)) / 2;
    y = (rows - 3) / 2;

    box(main_win, 0, 0);
    mvwprintw(main_win, y, x, "%s", prompt);

    // Print choices
    y = rows / 2;
    x = (cols - choice_size * NO_OPTIONS) / 2;

    // No highlighting
    if (!which)
    {
        mvwprintw(main_win, y, x, "%s%s", not_highlighted[0], not_highlighted[1]);
    }
    // Highlight 1st choice
    else if (which == 1)
    {
        // 1st choice
        wattron(main_win, A_BOLD | A_REVERSE);
        mvwprintw(main_win, y, x, "%s", highlighted[0]);
        wattroff(main_win, A_BOLD | A_REVERSE);

        // 2nd choice
        mvwprintw(main_win, y, x + choice_size, "%s", not_highlighted[1]);
    }
    // Highlight 2nd choice
    else if (which == 2)
    {
        // 1st choice
        mvwprintw(main_win, y, x, "%s", not_highlighted[0]);

        // 2nd choice
        wattron(main_win, A_BOLD | A_REVERSE);
        mvwprintw(main_win, y, x + choice_size, "%s", highlighted[1]);
        wattroff(main_win, A_BOLD | A_REVERSE);
    }

    wrefresh(main_win);
}