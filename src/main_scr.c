/* Main game screen */
#include <ncurses.h>
#include <string.h>

#include "engine.h"
#include "game_windows.h"
#include "moves.h"

/* DEFINITIONS */
#define BOARDS_WIN 0 
#define MENU_WIN   1 

#define NO_BOARDS 3

// Menu choices
#define RESTART  0
#define CONTINUE 1
#define UNDO     2
#define REDO     3
#define SAVE     4
#define LOAD     5
#define STATS    6
#define QUIT     7

// Game boards -> if element = 0 -> empty space, 1 -> X
int boards[NO_BOARDS][3][3];

// Determine if a board is dead -> 1: dead, 0: not
int dead_boards[NO_BOARDS];

// Number of games played by user + no. of wins & no. of loses
int engine_games[2];          // 0: wins,      1: loses
int two_user_games[2];        // 0: p 1 wins,  1: p 2 wins

int which_mode;

// turn =  1: computer or player 1
//      = -1: User     or player 2
int turn;

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

int play_two_user(int loaded);
int play_compu(int loaded);

int get_user_move();

int navigate_boards(int ch, int *x_pr, int *y_pr, int *menu_choice);
int use_menu();
int use_side_menu(int which_win);

void fill_boards();

void initial_msg();

int new_or_load();
int choose_mode();
int playing_order();
int play_again(int who_won);

int navigate_two_choices(int ch, int *which_pr);
void print_options(WINDOW *which_win, char *prompt, char *highlighted[], char *not_highlighted[], int which);

// Initialize game
void init_game()
{
    // Create windows needed in game
    int ch;
    do {
        endwin();
        refresh();

        if (create_windows())
        {
            break;
        }
        else
        {
            clear();
            print_error(7, 2);
        }
    }while ((ch = getch()));

    for (int i = 0; i < 2; i++)
    {
        engine_games[i] = two_user_games[i] = 0;
    }

    // Display static windows
    print_logo();
    print_instructions();

    initial_msg();

    // Play games until user quits
    int who_won;
    const int HUMAN_MODE = 0;
    const int COMPU_MODE = 1;

    do {
        // Initialize undo & redo stacks
        init_stacks();

        // New or loaded game
        int loaded_game = 0;
        if (new_or_load())
        {
            if (load_game())
            {
                loaded_game = 1;
                goto loaded;
            }
        }

        // Prompt user for mode
        which_mode = choose_mode();

        // Fill boards with 0
        fill_boards();
   
    loaded:
        // Clear main window & redraw borders
        wclear(main_win);
        box(main_win, 0, 0);
        wrefresh(main_win);

        // Play game & get winner
        if (which_mode == HUMAN_MODE)
        {
            who_won = play_two_user(loaded_game);
        }
        else if (which_mode == COMPU_MODE)
        {
            who_won = play_compu(loaded_game);
        }

        clear_stacks();

    }while (who_won == 2 || !play_again(who_won));
    
    destroy_windows();
}

// Two user game -> return winner
int play_two_user(int loaded)
{
    // Set turn for a new game
    if (!loaded)
    {
        turn = 1;
    }

    // Display initial state of windows
    print_boards(-1, -1);
    print_side_menu(BOARDS_WIN, 0);

    // Play moves until the game ends or user restarts
    while (!is_finished())
    {
        wclear(error_win);
        wrefresh(error_win);

        print_status(turn);

        // Prompt user for move
        if (get_user_move())
        {
            // Restart
            return 2;
        }
       
        print_boards(-1 , -1);
        turn *= -1;
    }

    // Update number of games
    if (turn == 1)
    {
        two_user_games[0]++;
    }
    else if (turn == -1)
    {
        two_user_games[1]++;
    }

    return turn;
}

// Game against engine -> return winner
int play_compu(int loaded)
{
    // Set turn & playing order
    // If the game is loaded -> user's turn
    turn = -1; 
    int order = 0;
    if (!loaded)
    {
        order = playing_order();
    }

    // Display initial state of windows
    wclear(main_win);
    box(main_win, 0, 0);
    wrefresh(main_win);

    print_boards(-1, -1);
    print_side_menu(BOARDS_WIN, 0);

    // If user chooses to play 2nd
    // play computer's 1st move
    if (order)
    {
        print_status(1);

        choose_move();
        print_boards(-1, -1);
    }

    // Play moves until game ends or user restarts
    while (!is_finished())
    {
        print_status(turn);

        // User to play
        if (turn == -1)
        {
            if (get_user_move())
            {
                // Restart
                return 2;
            }
        }
        // Engine to play
        else if (turn == 1)
        {
            choose_move();
        }

        turn *= -1;
        print_boards(-1, -1);
    }

    // Update number of games
    if (turn == 1)
    {
        engine_games[0]++;
    }
    else if (turn == -1)
    {
        engine_games[1]++;
    }

    return turn;
}

// Get a user move & play it
// Returns 2: restart, 0: otherwise
int get_user_move()
{
    // Navigate through boards & take user input
    int x, y, ch, menu_choice;
    x = y = 0;
    menu_choice = -1;
    while ((ch = getch()))
    {
        // Clear error window 
        wclear(error_win);
        wrefresh(error_win);

        redrawwin(status_win);
        wrefresh(status_win);

        // Check if user choose a move
        if (navigate_boards(ch, &x, &y, &menu_choice))
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
                // Invalid move
                if (dead_boards[x / 3])
                {
                    print_error(0, 0);
                }
                else
                {
                    print_error(1, 0);
                }
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
                case UNDO:
                    undo();
                    break;
                case REDO:
                    redo();
                    break;
                case SAVE:
                    save_game();
                    break;
                case LOAD:
                    load_game();
                    break;
                case STATS:
                    print_stats(engine_games, two_user_games);
                    break;
                case QUIT:
                    exit_game(0);
                    break;
            }

            // Re-set menu choice
            menu_choice = -1;

            // Re-print boards
            wclear(main_win);
            box(main_win, 0, 0);
            wrefresh(main_win);

            redrawwin(error_win);
            wrefresh(error_win);

            print_side_menu(BOARDS_WIN, 0);
            print_status(turn);
        }

        // Print game boards
        print_boards(x, y);
    }

next_move:
    return 0;
}

// Navigate between boards
// Returns 1: if user made a choice, 0: otherwise
int navigate_boards(int ch, int *x_pr, int *y_pr, int *menu_choice)
{
    // Set variables for use inside function
    const int BOARDS_HEIGHT = 3;
    const int BOARDS_WIDTH  = BOARDS_HEIGHT * NO_BOARDS;
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
               print_error(4, 0);
               y++;
            }
            break;
        // Move down
        case KEY_DOWN:
        case 'j':
            y++;
            // Check for borders
            if (y > BOARDS_HEIGHT - 1)
            {
                print_error(4, 0);
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
                print_error(4, 0);
                x++;
            }
            break;
        // Move right
        case KEY_RIGHT:
        case 'l':
            x++;
            // Check for borders
            if (x > BOARDS_WIDTH - 1)
            {
                print_error(4, 0);
                x--;
            }
            break;
        // Use side menu
        case 's':
            switch (use_side_menu(BOARDS_WIN))
            {
                case MENU_WIN:
                    *menu_choice = use_menu();
                    return 0;
                    break;
                case BOARDS_WIN:
                    print_side_menu(BOARDS_WIN, 0);
                    break;
            }
            break;
        // User quit
        case 'q':
            *menu_choice = QUIT;
            break;
        // User made a choice -> enter
        case 10:
            // Check if number is valid
            if (x > BOARDS_WIDTH - 1 || x < 0 || y > BOARDS_HEIGHT - 1 || y < 0)
            {
                print_error(2, 0);
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
            print_side_menu(BOARDS_WIN, 0);
            print_status(turn);

            break;
        // Invalid key
        default:
            print_error(3, 0);
    }

    // Re-set pointers
    *x_pr = x;
    *y_pr = y;

    return 0;
}

// Use menu & return user choice
int use_menu()
{
    const int NO_MENU_CHOICES = 8;

    // Print on top of boards win
    wclear(main_win);
    box(main_win, 0, 0);
    wrefresh(main_win);

    print_side_menu(MENU_WIN, 0);
    wrefresh(side_menu_win);

    print_menu(-1);
    
    // Navigate through menu & take user choice
    int ch, which;
    which = -1;

    while ((ch = getch()))
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
                switch (use_side_menu(MENU_WIN))
                {
                    case BOARDS_WIN:
                        return CONTINUE;
                        break;
                    case MENU_WIN:
                        print_side_menu(MENU_WIN, 0);
                        break;
                }
                break;
            // User quit
            case 'q':
                return QUIT;
                break;
            // User made a choice -> enter key
            case 10:
                // Check choice
                if (which < 0 || which > NO_MENU_CHOICES - 1)
                {
                    print_error(2, 0);
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
                print_side_menu(MENU_WIN, 0);

                break;
            // Invalid key
            default:
                print_error(3, 0);
        }

        // Print menu
        print_menu(which);
    }

    return 0;
}

// Use side menu -> choose shown window
int use_side_menu(int which_win)
{
    // Print side menu with highlighting
    int NO_MENU_CHOICES = 2;
    int navigate = which_win;
    print_side_menu(navigate, 1);

    // Navigate through choices & take user choice
    int ch;
    while ((ch = getch()))
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
                if (navigate > NO_MENU_CHOICES - 1)
                {
                    navigate--;
                }
                break;
            // User quit
            case 'q':
                exit_game(0);

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
                print_error(3, 0);
        }
        
        // Print choices
        print_side_menu(navigate, 1);
    }

    return 0;
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
    int ROWS, COLS, x, y;
    getmaxyx(main_win, ROWS, COLS);

    y = ROWS / 2;
    x = (COLS - strlen(start_msg)) / 2;

    // Print start message
    box(main_win, 0, 0);
    mvwprintw(main_win, y, x, "%s", start_msg);

    wrefresh(main_win);

    resize_or_quit(getch());

    wclear(main_win);
}

// Prompt for a new game
// Returns 1: load previous game, 0: new game
int new_or_load()
{
    char *choose[] =             {"|      Start new game     |", "|    Load previous game   |"};
    char *choose_highlighted[] = {"/      Start new game     /", "/    Load previous game   /"};

    // Print initial state of choices
    box(main_win, 0, 0);
    print_options(main_win, "", choose_highlighted, choose, 0);
    wrefresh(main_win);

    // Take user choice
    int ch, which;
    which = 0;
    while ((ch = getch()))
    {
        // Check if user made a choice
        if (navigate_two_choices(ch, &which))
        {
            return which - 1;
        }

        // Print choices with highlighting
        box(main_win, 0, 0);
        print_options(main_win, "", choose_highlighted, choose, which);

        redrawwin(error_win);
        wrefresh(error_win);
    }

    return 0;
}

// Prompt user for playing mode  
// Returns 1: against computer, 0: two player game
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
    while ((ch = getch()))
    {
        // Check if user made a choice
        if (navigate_two_choices(ch, &which))
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

    return 0;
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
    while ((ch = getch()))
    {
        // Check if user made a choice
        if (navigate_two_choices(ch, &which))
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

    return 0;
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
    while ((ch = getch()))
    {
        // Check if user made a choice
        if (navigate_two_choices(ch, &which))
        {
            return which - 1;
        }

        // Print choices with highlighting
        print_options(endgame_win, prompt, choices_highlighted, choices, which);
    }

    return 0;
}

// Navigate between choices & highlight choice
// Returns 1 if user made a choice, 0 otherwise
int navigate_two_choices(int ch, int *which_pr)
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
        // User quit
        case 'q':
            exit_game(0);
            break;
        // Enter key -> made a choice
        case 10:
            if (!which)
            {
                print_error(2, 0);
            }
            else
            {
                *which_pr = which;
                return 1;
            }
            break;
        // Terminal was resized
        case KEY_RESIZE:
            adjust_windows();
            break;
        // Invalid key
        default:
            print_error(3, 0);
    }

    // Re-set pointer
    *which_pr = which;
    return 0;
}

// Print choice options -> highlight position
// If which : 1 -> highlight 1st, 2 -> highlight 2nd, 0 -> no highlighting
void print_options(WINDOW *which_win, char *prompt, char *highlighted[], char *not_highlighted[], int which)
{
    const int NO_OPTIONS = 2;
    const int CHOICE_BUTTON_SIZE = 27;

    // Get window size &  printing position
    int ROWS, COLS, y, x;
    getmaxyx(which_win, ROWS, COLS);

    x = (COLS - strlen(prompt)) / 2;
    y = (ROWS - 3) / 2;

    mvwprintw(which_win, y, x, "%s", prompt);

    // Print choices
    y = ROWS / 2;
    x = (COLS - CHOICE_BUTTON_SIZE * NO_OPTIONS) / 2;

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
