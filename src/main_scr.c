/* Main game screen */
#include <ncurses.h>
#include <string.h>

#include "game_windows.h"
#include "moves.h"

/* DEFINITIONS */
#define HUMAN_MODE 0
#define COMPU_MODE 1

#define BOARDS_WIN 0 
#define MENU_WIN   1 

#define NO_BOARDS 3
#define NO_MENU_CHOICES 7         // Number of menu choices
#define NO_OPTIONS 2              // Number of options in opening / closing prompts

#define CHOICE_BUTTON_SIZE 27

// Menu choices
#define RESTART  0
#define CONTINUE 1
#define UNDO     2
#define REDO     3
#define STATS    4
#define SAVE     5 
#define QUIT     6 

// Game boards -> if element = 0 -> empty space, 1 -> X
int boards[NO_BOARDS][3][3];

// Determine if a board is dead -> 1: dead, 0: not
int dead_boards[NO_BOARDS];

// Number of games played by user + no. of wins & no. of loses
// 0: engine games, 1: 2 player games
int no_games[2];
int no_wins[2];
int no_loses[2];

int which_mode;

extern WINDOW *main_win;
extern WINDOW *boards_win[NO_BOARDS];
extern WINDOW *menu_win;
extern WINDOW *side_menu_win;
extern WINDOW *error_win;
extern WINDOW *status_win;
extern WINDOW *stats_win;
extern WINDOW *endgame_win;

/* FUNCTIONS */
void init_game();

int play_two_user();
int play_compu();

int navigate_boards(int ch, int *x_pr, int *y_pr, int *menu_choice, int turn);

int use_menu();
int use_side_menu(int which_win, int turn);

void fill_boards();

void initial_msg();

int choose_mode();
int playing_order();
int play_again(int who_won);

int navigate(int ch, int *which_pr);

void print_options(WINDOW *which_win, char *prompt, char *highlighted[], char *not_highlighted[], int which);

// Initialize game
void init_game()
{
    // Create windows needed in game
    int ch;
    do {
        if (create_windows())
        {
            break;
        }
        else
        {
            clear();
            print_error(-1);
        }
    }while ((ch = getch()) == KEY_RESIZE);

    for (int i = 0; i < 2; i++)
    {
        no_games[i] = no_wins[i] = no_loses[i] = 0;
    }

    // Display static windows
    print_logo();
    print_instructions();

    initial_msg();

    // Play games until user quits
    for (;;)
    {
        // Prompt user for mode
        which_mode = choose_mode();
    
        // Clear main window & redraw borders
        wclear(main_win);
        box(main_win, 0, 0);
        wrefresh(main_win);

        // Fill boards with 0
        fill_boards();

        int who_won;

        if (which_mode == HUMAN_MODE)
        {
            who_won = play_two_user();
        }
        else if (which_mode == COMPU_MODE)
        {
            who_won = play_compu();
        }

        // User quit or Game ended 
        if (!who_won || (who_won != 2 && play_again(who_won)))
        {
            break;
        }
    }
}

// Two user game -> return winner
int play_two_user()
{
    // Set turn & increase no. of games
    int turn = 1;
    no_games[1]++;

    // Display initial state of windows
    print_boards(-1, -1);
    print_side_menu(BOARDS_WIN);

    while (!is_finished())
    {
        wclear(error_win);
        wrefresh(error_win);

        print_status(turn);

        // Navigate through boards & take user input
        int x, y, ch, menu_choice;
        x = y = 0;
        menu_choice = -1;
        while ((ch = getch()) != 'q')
        {
            // Clear error window 
            wclear(error_win);
            wrefresh(error_win);

            redrawwin(status_win);
            wrefresh(status_win);

            // Check if user made a choice
            if (navigate_boards(ch, &x, &y, &menu_choice, turn))
            {
                // Validate move
                if (is_valid(x, y))
                {
                    // Play move 
                    play_move(x, y);

                    goto next_move;
                }
                else
                {
                    print_error(0);
                }
            }
            // If user made a menu choice
            else if (menu_choice != -1)
            {
                // Check choice
                switch (menu_choice)
                {
                    case RESTART:
                        return 2;
                        break;
                    case CONTINUE:
                        break;
/*                  case UNDO:
 *                  case REDO:*/
                    case STATS:
                        print_stats(no_games, no_wins, no_loses);
                        break;
/*                  case SAVE:*/
                    case QUIT:
                        return 0;
                        break;
                }

                // Re-set menu choice
                menu_choice = -1;

                // Re-print boards
                wclear(main_win);
                box(main_win, 0, 0);
                wrefresh(main_win);

                print_side_menu(BOARDS_WIN);
                print_status(turn);
            }

            // Print game boards
            print_boards(x, y);
        }

    next_move:
        // Check if user quit game
        if (ch == 'q')
        {
            return 0;
        }
        
        print_boards(-1 , -1);
        turn *= -1;
    }

    return turn;
}

// Game against engine
int play_compu()
{
    // Set turn & increase no. of games 
    int turn = 1;
    no_games[1]++;

    // Choose playing order
    if (playing_order())
    {
        // Engine plays 1st move
        // ...
    }

    // Display initial state of windows
    wclear(main_win);
    box(main_win, 0, 0);
    wrefresh(main_win);

    print_boards(-1, -1);
    print_side_menu(BOARDS_WIN);

    getch();
    return 0;
    // ...
}

// Navigate between boards
// Returns 1: if user made a choice, 0: otherwise
int navigate_boards(int ch, int *x_pr, int *y_pr, int *menu_choice, int turn)
{
    // Set variables for use inside function
    int x, y;
    x = *x_pr;
    y = *y_pr;

    // Check user choice
    switch (ch)
    {
        // Move up
        case KEY_UP:
        case 'k':
            y--;
            // Check for borders
            if (y < 0)
            {
               print_error(4);
               y++;
            }
            break;
        // Move down
        case KEY_DOWN:
        case 'j':
            y++;
            // Check for borders
            if (y > 2)
            {
                print_error(4);
                y--;
            }
            break;
        // Move left
        case KEY_LEFT:
        case 'h':
            x--;
            // Check for borders
            if (x < 0)
            {
                print_error(4);
                x++;
            }
            break;
        // Move right
        case KEY_RIGHT:
        case 'l':
            x++;
            // Check for borders
            if (x > 8)
            {
                print_error(4);
                x--;
            }
            break;
        // Use side menu
        case 's':
            if (use_side_menu(BOARDS_WIN, turn) == MENU_WIN)
            {
                // Take user choice
                *menu_choice = use_menu();
                return 0;
            }
            break;
        // User made a choice -> enter
        case 10:
            // Check if number is valid
            if (x > 8 || x < 0 || y > 2 || y < 0)
            {
                print_error(3);
            }
            else
            {
                // Re-set pointers
                *x_pr = x;
                *y_pr = y;
                
                return 1;
            }
            break;
        // Terminal resized
        case KEY_RESIZE:
            // Re-create windows
            adjust_windows();

            // Re-print windows
            print_side_menu(BOARDS_WIN);
            print_status(turn);

            break;
        // Invalid key
        default:
            print_error(2);
    }

    // Re-set pointers
    *x_pr = x;
    *y_pr = y;

    return 0;
}

// Use menu & return user choice
int use_menu()
{
    // Print menu on top of boards win
    wclear(main_win);
    box(main_win, 0, 0);
    wrefresh(main_win);

    print_side_menu(MENU_WIN);
    wrefresh(side_menu_win);

    print_menu(-1);
    
    // Navigate through menu & take user choice
    int ch, which;
    which = -1;

    while ((ch = getch()) != 'q')
    {
        wclear(error_win);
        wrefresh(error_win);

        switch (ch)
        {
            case KEY_UP:
            case 'k':
                which--;
                // Check border
                if (which < 0)
                {
                    which = 0;
                }
                break;
            case KEY_DOWN:
            case 'j':
                which++;
                // Check border
                if (which > NO_MENU_CHOICES - 1)
                {
                    which = NO_MENU_CHOICES - 1;
                }
                break;
            // Use side menu
            case 's':
                if (use_side_menu(MENU_WIN, 0) == BOARDS_WIN)
                {
                    return CONTINUE;
                }
                break;
            // Enter key
            case 10:
                // Check choice
                if (which < 0 || which > NO_MENU_CHOICES - 1)
                {
                    print_error(1);
                }
                else
                {
                    return which;
                }
                break;
            // Terminal resized
            case KEY_RESIZE:
                // Re-create windows
                adjust_windows();
                print_side_menu(MENU_WIN);

                break;
            // Invalid key
            default:
                print_error(2);
        }

        // Print menu
        print_menu(which);
    }

    // Check if user quit
    if (ch == 'q')
    {
        return QUIT;
    }
}

// Use side menu -> choose shown window
int use_side_menu(int which_win, int turn)
{
    int navigate = which_win;

    // Navigate through choices & take user choice
    int ch;
    while ((ch = getch()) != 'q')
    {
        wclear(side_menu_win);
        wclear(error_win);
        wrefresh(error_win);

        switch (ch)
        {
            case KEY_UP:
            case 'k':
                navigate--;
                // Check borders
                if (navigate < 0)
                {
                    navigate++;
                }
                break;
            case KEY_DOWN:
            case 'j':
                navigate++;
                // Check borders
                if (navigate > 1)
                {
                    navigate--;
                }
                break;
            // Enter key -> user made a choice
            case 10:
                return navigate;
            // Terminal resized
            case KEY_RESIZE:
                // Re-create windows
                adjust_windows();

                // Re-print windows
                if (which_win == BOARDS_WIN)
                {
                    print_boards(-1, -1);
                    print_status(turn);
                }
                else if (which_win == MENU_WIN)
                {
                    print_menu(-1);
                }
                break;
            // Invalid key
            default:
                print_error(2);
        }
        
        // Print choices
        print_side_menu(navigate);
    }
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

        dead_boards[i] = 0;
    }
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
    if (getch() == KEY_RESIZE)
    {
        adjust_windows();
    }

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
    box(main_win, 0, 0);
    print_options(main_win, prompt, modes_highlighted, modes, 0);
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
        print_options(main_win, prompt, modes_highlighted, modes, which);

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
    box(main_win, 0, 0);
    print_options(main_win, prompt, orders_highlighted, orders, 0);
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
        print_options(main_win, prompt, orders_highlighted, orders, which);

        // Re-draw error window
        redrawwin(error_win);
        wrefresh(error_win);
    }
}

// Prompt user for another game & print game ending message
int play_again(int who_won)
{
    char *prompt = "Play again?"; 
    char *choices[] =             {"|        New  game        |", "|           Quit          |"};
    char *choices_highlighted[] = {"/        New  game        /", "/           Quit          /"};

    // Print message
    print_end_msg(who_won);

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
            which = 1;
            break;
        // Move right
        case KEY_RIGHT:
        case 'l':
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
        // Terminal was resized
        case KEY_RESIZE:
            adjust_windows();
            break;
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
void print_options(WINDOW *which_win, char *prompt, char *highlighted[], char *not_highlighted[], int which)
{
    // Get window size &  printing position
    int rows, cols, y, x;
    getmaxyx(which_win, rows, cols);

    // Print prompt
    x = (cols - strlen(prompt)) / 2;
    y = (rows - 3) / 2;

    mvwprintw(which_win, y, x, "%s", prompt);

    // Print choices
    y = rows / 2;
    x = (cols - CHOICE_BUTTON_SIZE * NO_OPTIONS) / 2;

    // No highlighting
    if (!which)
    {
        mvwprintw(which_win, y, x, "%s%s", not_highlighted[0], not_highlighted[1]);
    }
    // Highlight 1st choice
    else if (which == 1)
    {
        // 1st choice
        wattron(which_win, A_BOLD | A_REVERSE);
        mvwprintw(which_win, y, x, "%s", highlighted[0]);
        wattroff(which_win, A_BOLD | A_REVERSE);

        // 2nd choice
        mvwprintw(which_win, y, x + CHOICE_BUTTON_SIZE, "%s", not_highlighted[1]);
    }
    // Highlight 2nd choice
    else if (which == 2)
    {
        // 1st choice
        mvwprintw(which_win, y, x, "%s", not_highlighted[0]);

        // 2nd choice
        wattron(which_win, A_BOLD | A_REVERSE);
        mvwprintw(which_win, y, x + CHOICE_BUTTON_SIZE, "%s", highlighted[1]);
        wattroff(which_win, A_BOLD | A_REVERSE);
    }

    wrefresh(which_win);
}
