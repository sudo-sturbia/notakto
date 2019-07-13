/* Initialization & opening prompts */
#include <ncurses.h>
#include <string.h>
#include "game_windows.h"

/* DEFINITIONS */
#define HUMAN_MODE 0
#define COMPU_MODE 1

#define NO_OPTIONS 2              // Number of options in opening prompts

#define CHOICE_BUTTON_SIZE 27

int which_mode;

extern WINDOW *main_win;
extern WINDOW *error_win;

/* FUNCTIONS */
void init_game();
void initial_msg();

int choose_mode();
int playing_order();

int navigate(int ch, int *which_pr);

void print_options(char *prompt, char *highlighted[], char *not_highlighted[], int which);

// Start game & take beginning choices
void init_game()
{
    // Create windows needed in game
    create_windows();

    // Display static windows
    print_logo();
    print_instructions();

    initial_msg();

    // Prompt user for mode
    which_mode = choose_mode();
}

// Print initial message to start game
void initial_msg()
{
    char *start_msg = "PRESS ANY KEY TO START";

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
}

// Prompt user to start game or display game guide
// Returns 1: start, 0: guide
int choose_mode()
{
    char *prompt = "Choose a playing mode";
    char *modes[] =             {"|       TWO PLAYERS       |", "|   PLAY vs THE MACHINE   |"};
    char *modes_highlighted[] = {"/       TWO PLAYERS       /", "/   PLAY vs THE MACHINE   /"};

    // Print initial state of choices
    print_options(prompt, modes_highlighted, modes, 0);
    wrefresh(main_win);

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
        box(main_win, 0, 0);
        print_options(prompt, modes_highlighted, modes, which);

        // Re-draw error window
        redrawwin(error_win);
        wrefresh(error_win);
    }
}

// Prompt user for playing order -> used in computer mode
int playing_order()
{
    char *prompt = "Choose playing order";
    char *orders[] =             {"|       PLAY  FIRST       |", "|       PLAY SECOND       |"};
    char *orders_highlighted[] = {"/       PLAY  FIRST       /", "/       PLAY SECOND       /"};

    // Print initial state of choices
    print_options(prompt, orders_highlighted, orders, 0);
    wrefresh(main_win);

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
        box(main_win, 0, 0);
        print_options(prompt, orders_highlighted, orders, which);

        // Re-draw error window
        redrawwin(error_win);
        wrefresh(error_win);
    }
}

// Navigate between choices & highlight choice
// Returns 1 if user made a choice, 0 otherwise
int navigate(int ch, int *which_pr)
{
    // Set which variable
    int which = *which_pr;

    // Clear main & error windows
    wclear(main_win);
    wclear(error_win);
    wrefresh(error_win);

    // Highlight choice
    switch(ch)
    {
        // Move left
        case KEY_LEFT:
        case 'h':
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
        case 'l':
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
                *which_pr = which;
                return 1;
            }
        // Invalid key
        default:
            print_error(2);
    }

    // Re-set pointer
    *which_pr = which;
    return 0;
}

// Print choice options -> highlight position
// If which : 1 -> highlight 1st, 2 -> highlight 2nd, 0 -> no highlighting
void print_options(char *prompt, char *highlighted[], char *not_highlighted[], int which)
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
    x = (cols - CHOICE_BUTTON_SIZE * NO_OPTIONS) / 2;

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
        mvwprintw(main_win, y, x + CHOICE_BUTTON_SIZE, "%s", not_highlighted[1]);
    }
    // Highlight 2nd choice
    else if (which == 2)
    {
        // 1st choice
        mvwprintw(main_win, y, x, "%s", not_highlighted[0]);

        // 2nd choice
        wattron(main_win, A_BOLD | A_REVERSE);
        mvwprintw(main_win, y, x + CHOICE_BUTTON_SIZE, "%s", highlighted[1]);
        wattroff(main_win, A_BOLD | A_REVERSE);
    }

    wrefresh(main_win);
}