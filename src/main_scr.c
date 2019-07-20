/* Main game screen */
#include <ncurses.h>
#include <string.h>
#include "initialization.h"
#include "moves.h"

/* DEFINITIONS */
#define HUMAN_MODE 0
#define COMPU_MODE 1

#define BOARDS_WIN 0 
#define MENU_WIN   1 

#define NO_BOARDS 3

// Menu choices
#define RESTART  0
#define CONTINUE 1
#define UNDO     2
#define REDO     3
#define SAVE     4
#define QUIT     5

// Game boards -> if element = 0 -> empty space, 1 -> X
int boards[NO_BOARDS][3][3];

// Determine if a board is dead -> 1: dead, 0: not
int dead_boards[NO_BOARDS];

// Number of games played by user + no. of wins & no. of loses
int no_games;
int no_wins;
int no_loses;

extern int which_mode;

extern WINDOW *main_win;
extern WINDOW *boards_win[NO_BOARDS];
extern WINDOW *menu_win;
extern WINDOW *side_menu_win;
extern WINDOW *error_win;
extern WINDOW *status_win;
extern WINDOW *endgame_win;

/* FUNCTIONS */
void play_game();

int play_two_user();
int play_compu();

int navigate_side_menu(int which_win);
int navigate_boards(int ch, int *x_pr, int *y_pr, int *menu_choice);
int use_menu();

void fill_boards();

void print_side_menu(int which_win);
void print_boards(int x, int y);
void print_board(int board[3][3], WINDOW *board_win);
void print_menu(int which);
void print_status(int turn);
int print_endgame(int who_won);
void print_error(int error_num);

// Play games -> both modes 
void play_game()
{
    no_games = no_wins = no_loses = 0;

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

        no_games++;

        // User quit or Game ended 
        if (!who_won || (who_won != 2 && print_endgame(who_won)))
        {
            break;
        }
    }
}

// Two user game -> return winner
int play_two_user()
{
    // Set turn 
    int turn = 1;

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
 *                  case REDO:
 *                  case SAVE:*/
                    case QUIT:
                        return 0;
                        break;
                }

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
}

// Game against engine
int play_compu()
{
    // Set turn variable
    int turn = 1;

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

// Use side menu -> choose shown window
int navigate_side_menu(int which_win)
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
            // Invalid key
            default:
                print_error(2);
        }
        
        // Print choices
        print_side_menu(navigate);
    }
}

// Navigate between boards
// Returns 1: if user made a choice, 0: otherwise
int navigate_boards(int ch, int *x_pr, int *y_pr, int *menu_choice)
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
            if (navigate_side_menu(BOARDS_WIN) == MENU_WIN)
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
                if (which > 5)
                {
                    which = 5;
                }
                break;
            // Enter key
            case 10:
                // Check choice
                if (which < 0 || which > 5)
                {
                    print_error(1);
                }
                else
                {
                    return which;
                }
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

/* Printing functions */

// Print side menu -> indicates which window is being used
void print_side_menu(int which_win)
{
    char *tag = " CURRENT-WINDOW:";
    char *windows[] = {" - Game",
                       " - Menu"};
    char *windows_highlighted[] = {"     -> Game    ",
                                   "     -> Menu    "};

    // Clear window & print borders
    wclear(side_menu_win);
    box(side_menu_win, 0, 0);

    // Get screen size
    int rows, cols;
    getmaxyx(side_menu_win, rows, cols);

    // Print tag
    mvwprintw(side_menu_win, 0, 1, "%s", tag);

    // Print menu
    for (int i = 0; i < 2; i++)
    {
        // Highlighted choice
        if (i == which_win)
        {
            wattron(side_menu_win, A_BOLD | A_REVERSE);
            mvwprintw(side_menu_win, i + 2, 2, "%s", windows_highlighted[i]);
            wattroff(side_menu_win, A_BOLD | A_REVERSE);
        }
        // Un-highlighted choice
        else
        {
            mvwprintw(side_menu_win, i + 2, 2, "%s", windows[i]);
        }
    }
    
    wrefresh(side_menu_win);
}

// Print game boards
// if 1 -> X, 0 -> empty space
void print_boards(int x, int y)
{
    // Print boards without highlighting
    for (int i = 0; i < NO_BOARDS; i++)
    {
        print_board(boards[i], boards_win[i]);
    }

    // Find which board has highlighted element
    int which_board;
    if (x > 5)
    {
        which_board = 2;
    }
    else if (x > 2)
    {
        which_board = 1;
    }
    else
    {
        which_board = 0;
    }

    x %= 3;

    // Calculate printing position
    int where_x, where_y;

    where_y = 1 + (2 * y);
    where_x = 4 * x;

    // Find highlighted element character -> X if 1, empty space if 0
    char highlighted;

    highlighted = (boards[which_board][y][x]) ? 'X' : ' ';

    // Print highlighted element
    wattron(boards_win[which_board], A_BOLD);

    mvwprintw(boards_win[which_board], where_y - 1, where_x, " +++ ");
    mvwprintw(boards_win[which_board], where_y, where_x, "/ %c /", highlighted);
    mvwprintw(boards_win[which_board], where_y + 1, where_x, " +++ ");

    wattroff(boards_win[which_board], A_BOLD);

    wrefresh(boards_win[0]);
    wrefresh(boards_win[1]);
    wrefresh(boards_win[2]);
}

// Print a single board inside a given window -> no highlighting
void print_board(int board[3][3], WINDOW *board_win)
{
    // Grid
    char *grid[] = {" --- --- --- ",
                    "|   |   |   |",
                    " --- --- --- ",
                    "|   |   |   |",
                    " --- --- --- ",
                    "|   |   |   |",
                    " --- --- --- "};

    int grid_width, grid_height;
    grid_height = 7;
    grid_width = 13;

    // Print grid
    for (int i = 0; i < grid_height; i++)
    {
        mvwprintw(board_win, i, 0, "%s", grid[i]);
    }

    // Print elements
    int x, y;
    x = y = 0;

    for (int i = 1; i < grid_height; i += 2)
    {
        x = 0;

        for (int j = 2; j < grid_width; j += 4)
        {
            // Print character 
            switch (board[y][x])
            {
                case 0:
                    mvwaddch(board_win, i, j, ' ');
                    break;
                case 1:
                    mvwaddch(board_win, i, j, 'X');
                    break;
            }

            x++;
        }

        y++;
    }
}

// Print choices menu -> with highlighting, choices start at 0
void print_menu(int which)
{
    char *tag = " MENU:";
    char *menu[] = {" - Restart",
                    " - Continue",
                    " - Undo last move",
                    " - Redo last move",
                    " - Save game",
                    " - Quit"};

    char *highlighted_menu[] = {"     -> Restart                         ",
                                "     -> Continue                        ",
                                "     -> Undo last move                  ",
                                "     -> Redo last move                  ",
                                "     -> Save game                       ",
                                "     -> Quit                            "};
    
    // Print borders & tag
    wclear(menu_win);
    box(menu_win, 0, 0);
    mvwprintw(menu_win, 0, 1, "%s", tag);

    // Print menu with choice highlighted
    for (int i = 0, no_choices = 6; i < no_choices; i++)
    {
        // Highlighted choice
        if (i == which)
        {
            wattron(menu_win, A_BOLD | A_REVERSE);
            mvwprintw(menu_win, i + 2, 5, "%s", highlighted_menu[i]);
            wattroff(menu_win, A_BOLD | A_REVERSE);
        }
        else
        {
            mvwprintw(menu_win, i + 2, 5, "%s", menu[i]);
        }
    }

    wrefresh(menu_win);
}

// Print turn status
// If turn = 1 -> computer or player 1, turn = -1 -> otherwise
void print_status(int turn)
{
    char *tag = " STATUS:";
    char *status[] = {"Player 1 to play",
                      "Player 2 to play",
                      "Thinking ...",
                      "Your turn"};

    // Clear status window & print borders
    wclear(status_win);
    box(status_win, 0, 0);

    // Print tag
    mvwprintw(status_win, 0, 1, "%s", tag);

    // Printing position
    int x, y;
    x = 4;
    y = 1;


    // Print turn status
    if (which_mode == COMPU_MODE)
    {
        // Computer turn
        if (turn == 1)
        {
            mvwprintw(status_win, y, x, "%s", status[2]);
        }
        // User turn 
        else if (turn == -1)
        {
            mvwprintw(status_win, y, x, "%s", status[3]);
        }
    }
    else if (which_mode == HUMAN_MODE)
    {
        // Player 1 turn 
        if (turn == 1)
        {
            mvwprintw(status_win, y, x, "%s", status[0]);
        }
        // User turn 
        else if (turn == -1)
        {
            mvwprintw(status_win, y, x, "%s", status[1]);
        }
    }

    wrefresh(status_win);
}

// Print message & prompt after game ending
// If who_won = 1 -> computer or player 1, who_won = -1 -> otherwise
int print_endgame(int who_won)
{
    char *prompt     =  "Play again?";
    char *win_msg[]  = {"Congratulations",
                        "PLAYER ?",
                        "You won"};
    char *lose_msg[] = {"You lost",
                        "Better luck next time"};

    char *choices[] =             {"|        New  game        |", "|           Quit          |"};
    char *choices_highlighted[] = {"/        New  game        /", "/           Quit          /"};

    // Get window size & printing position
    int rows, cols, x1, x2;
    getmaxyx(endgame_win, rows, cols);

    // Clear windows
    wclear(endgame_win);
    wclear(main_win);

    box(main_win, 0, 0);

    wrefresh(endgame_win);
    wrefresh(main_win);

    // Print game ending message
    if (which_mode == COMPU_MODE)
    {
        // User won
        if (who_won == 1)
        {
            x1 = (cols - strlen(win_msg[0])) / 2;
            x2 = (cols - strlen(win_msg[2])) / 2;

            mvwprintw(endgame_win, 0, x1, "%s", win_msg[0]);
            mvwprintw(endgame_win, 1, x2, "%s", win_msg[2]);
        }
        // Computer won
        else if (who_won == -1)
        {
            x1 = (cols - strlen(lose_msg[0])) / 2;
            x2 = (cols - strlen(lose_msg[1])) / 2;

            mvwprintw(endgame_win, 0, x1, "%s", lose_msg[0]);
            mvwprintw(endgame_win, 1, x2, "%s", lose_msg[1]);
        }
    }
    else if (which_mode == HUMAN_MODE)
    {
        // Find winner 
        char *winner = (who_won == -1) ? "PLAYER 1" : "PLAYER 2";

        x1 = (cols - strlen(win_msg[0])) / 2;
        x2 = (cols - strlen(win_msg[1])) / 2;

        mvwprintw(endgame_win, 0, x1, "%s", win_msg[0]);
        mvwprintw(endgame_win, 1, x2, "%s", winner);
    }

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

// Print error messages
void print_error(int error_num)
{
    // Error messages
    char *tag = "Error: ";
    char *error_msgs[] = {"invalid move", "no choice made", "invalid key", "invalid choice", "border"};

    // Get window size & printing position
    int rows, cols, y, x;
    getmaxyx(error_win, rows, cols);

    x = (cols - strlen(error_msgs[error_num])) / 2;
    y = 1;

    // Clear error window
    wclear(error_win);

    // Print error message
    mvwprintw(error_win, y, x, "%s%s", tag, error_msgs[error_num]);

    wrefresh(error_win);
}
