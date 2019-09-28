/* Windows used in game */
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "moves.h"

/* DEFINITIONS */
#define HUMAN_MODE 0
#define COMPU_MODE 1

#define BOARDS_WIN 0 
#define MENU_WIN   1 

#define NO_BOARDS 3

#define LOGO_WIDTH 62
#define LOGO_HEIGHT 5

/* WINDOWS */
WINDOW *main_win;
WINDOW *boards_win[NO_BOARDS];
WINDOW *logo_win;
WINDOW *instructions_win;
WINDOW *menu_win;
WINDOW *side_menu_win;
WINDOW *error_win;
WINDOW *status_win;
WINDOW *stats_win;
WINDOW *endgame_win;

extern int boards[NO_BOARDS][3][3];
extern int which_mode;

/* FUNCTIONS */
int create_windows();
void clear_windows();
void destroy_windows();

void exit_game(int code);

void adjust_windows();
void resize_or_quit(int ch);

void print_logo();
void print_instructions();

void print_side_menu(int which_win, int is_used);
void print_boards(int x, int y);
void print_board(int board[3][3], WINDOW *board_win);
void print_menu(int which);
void print_status(int turn);
void print_stats(int engine_games[2], int two_user_games[2]);
void print_end_msg(int who_won);
void print_error(int error_num, int which_win);

// Create windows used in game
// Returns 1: windows created, 0: otherwise
int create_windows()
{
    // Minimum terminal size to play game
    const int M_WIDTH  = 96;
    const int M_HEIGHT = 24;

    // Get screen size
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    if (rows >= M_HEIGHT && cols >= M_WIDTH)
    {
        // Variables to represent printing position & window size
        int x, y, height, width;

        // Create logo window
        height = 9;
        width = 65;
        y = rows - 9;
        x = 0;

        logo_win = newwin(height, width, y, x);

        // Create instructions window
        height = 9;
        width = cols - 66;
        y = rows - 9;
        x = 66;

        instructions_win = newwin(height, width, y, x);

        // Create main game window
        height = rows - 9;
        width = cols;
        y = 0;
        x = 0;

        main_win = newwin(height, width, y, x);

        // Create side menu window 
        height = 6;
        width = 20;
        y = 2;
        x = 3;

        side_menu_win = newwin(height, width, y, x);

        // Create boards windows -> inside main window
        height = 7;
        width = 13;
        y = (rows - height - 9) / 2;
        x = (cols - (3 * width + 2 * 8)) / 2;

        x = (x > (20 + 3)) ? x : (20 + 3 + 2);             //  Boards & side menu don't overlap

        boards_win[0] = newwin(height, width, y, x);
        boards_win[1] = newwin(height, width, y, x + width * 1 + 8 * 1);
        boards_win[2] = newwin(height, width, y, x + width * 2 + 8 * 2);

        // Create menu window -> inside main window
        height = 12;
        width = 50;
        y = (rows - height - 9) / 2;
        x = (cols - width) / 2;

        menu_win = newwin(height, width, y, x);

        // Create error window -> inside main window
        height = 1;
        width = 35;
        y = rows - 9 - 2;
        x = (cols - width) / 2;

        error_win = newwin(height, width, y, x);

        // Create status (turn) window -> inside main window
        height = 3;
        width = 24;
        y = rows - 9 - 4;
        x = 2;

        status_win = newwin(height, width, y, x);

        // Create stats window -> inside main window
        height = 10;
        width = 58;
        y = (rows - height - 9) / 2;
        x = (cols - width) / 2;

        stats_win = newwin(height, width, y, x);

        // Create game ending window -> inside main window
        height = 10;
        width = 60;
        y = (rows - height - 9) / 2;
        x = (cols - width) / 2;

        endgame_win = newwin(height, width, y, x);

        return 1;
    }
    else
    {
        // Terminal size is too small
        return 0;
    }
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

// Exit game
void exit_game(int code)
{
    // Create a window to print exit message
    WINDOW *exit_win;
    char *sure[] = {"Are you sure?",
                    "Press q to exit, Any other key to return."};

    int ch, y, x, COLS, ROWS;

    getmaxyx(stdscr, ROWS, COLS);

    const int HEIGHT = 5;
    const int WIDTH  = 45; 

    y = (ROWS - HEIGHT - 9) / 2; 
    x = (COLS - 45) / 2;

    exit_win = newwin(HEIGHT, WIDTH, y, x);

    box(exit_win, 0, 0);
    mvwprintw(exit_win, 1, (WIDTH - strlen(sure[0])) / 2, "%s", sure[0]);
    mvwprintw(exit_win, 3, (WIDTH - strlen(sure[1])) / 2, "%s", sure[1]);
    wrefresh(exit_win);

    ch = getch();

    if (ch == 'q')
    {
        delwin(exit_win);
        destroy_windows();
        clear_stacks();

        endwin();
        exit(code);
    }
    else 
    {
        resize_or_quit(ch);   // Detects only resizing

        wclear(exit_win);
        wrefresh(exit_win);
        delwin(exit_win);

        return;
    }
}

/* Deal with terminal resizing */
// which_win -> indicates whether boards or menu is being displayed
// x, y -> which choice to highlight 
void adjust_windows()
{
    // Remove all windows
    clear_windows();
    destroy_windows();

    // Re-create windows
    int ch;
    do{
        // Re-initialize curses mode
        endwin();
        refresh();

        // Check if windows were created correctly
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

    // Re-print windows
    box(main_win, 0, 0);
    wrefresh(main_win);

    print_logo();
    print_instructions();
}

// Detect resizing or quiting 
void resize_or_quit(int ch)
{
    if (ch == KEY_RESIZE)
    {
        adjust_windows();
    }
    else if (ch == 'q')
    {
        exit_game(0);
    }
}

/* Printing functions */

/* Static windows */
void print_logo()
{
    // Logo & author
    // Logo is represented using an integer array where -> 0: ' ', 1: block, 2: |, 3: -
    char *author = "@sudo-sturbia";
    int logo_arr[LOGO_HEIGHT][LOGO_WIDTH] = {{1, 1, 1, 2, 0, 0, 0, 1, 1, 2, 0, 1, 1, 1, 1, 1, 1, 2, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 1, 1,
                                              1, 1, 1, 2, 0, 1, 1, 2, 0, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 1, 1, 1, 1, 1, 1, 2, 0},
                                             {1, 1, 1, 1, 2, 0, 0, 1, 1, 2, 1, 1, 2, 3, 3, 3, 1, 1, 2, 0, 3, 3, 1, 1, 2, 3, 3, 0, 1, 1, 2,
                                              3, 3, 1, 1, 2, 1, 1, 2, 0, 1, 1, 2, 0, 0, 3, 3, 1, 1, 2, 3, 3, 0, 1, 1, 2, 3, 3, 3, 1, 1, 2},
                                             {1, 1, 2, 1, 1, 2, 0, 1, 1, 2, 1, 1, 2, 0, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 1,
                                              1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 2},
                                             {1, 1, 2, 0, 1, 1, 2, 1, 1, 2, 1, 1, 2, 0, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 2,
                                              3, 3, 1, 1, 2, 1, 1, 2, 3, 1, 1, 2, 0, 0, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 2},
                                             {1, 1, 2, 0, 0, 1, 1, 1, 1, 2, 0, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 2,
                                              0, 0, 1, 1, 2, 1, 1, 2, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 2, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 0}
                                             };

    // Logo window size
    const int WIN_ROWS = 9;
    const int WIN_COLS = 65;

    // Print borders
    box(logo_win, 0, 0);

    // Print logo
    for (int i = 0; i < LOGO_HEIGHT; i++)
    {
        for (int j = 0; j < LOGO_WIDTH; j++)
        {
            switch(logo_arr[i][j])
            {
                case 0:
                    mvwaddch(logo_win, i + 2, j + 2, ' ');
                    break;
                case 1:
                    mvwaddch(logo_win, i + 2, j + 2, ' ' | A_REVERSE);
                    break;
                case 2:
                    mvwaddch(logo_win, i + 2, j + 2, '|');
                    break;
                case 3:
                    mvwaddch(logo_win, i + 2, j + 2, '-');
                    break;
            }
        }
    }

    // Print author
    int y, x;
    y = WIN_ROWS - 2;
    x = WIN_COLS - strlen(author) - 5;

    mvwprintw(logo_win, y, x, "%s", author);

    wrefresh(logo_win);
}

// Print game instructions 
void print_instructions()
{
    const int NO_LINES = 8;
    char *instructions[] = {" INSTRUCTIONS:",
                            "\n",
                            " to navigate        ^",
                            "            < h  j  k  l >",
                            "                 v",
                            " for side menu    s",
                            " to quit          q",
                            " to make a choice ENTER"};

    // Get window size & printing position
    const int WIN_COLS = getmaxx(instructions_win);

    int x;
    x = WIN_COLS / 8;
    x = (x > 0) ? x : 1;           // x must be higher than 0

    // Print instructions
    for (int i = 1; i < NO_LINES; i++)
    {
        mvwprintw(instructions_win, i, x, "%s", instructions[i]);
    }

    // Print borders
    box(instructions_win, 0, 0);

    // Print window tag
    mvwprintw(instructions_win, 0, 1, "%s", instructions[0]);

    wrefresh(instructions_win);
}

// Print side menu -> indicates which window is being used
void print_side_menu(int which_win, int is_used)
{
    char *tag = " CURRENT-WINDOW:";
    char *windows[] = {" - Game",
                       " - Menu"};
    char *windows_highlighted[] = {"     -> Game    ",
                                   "     -> Menu    "};
    const int NO_CHOICES = 2;

    // Clear window & print borders
    wclear(side_menu_win);
    box(side_menu_win, 0, 0);

    // Highlight tag if side menu is being used
    if (is_used)
    {
        wattron(side_menu_win, A_BOLD | A_ITALIC);
    }

    // Print tag
    mvwprintw(side_menu_win, 0, 1, "%s", tag);

    if (is_used)
    {
        wattroff(side_menu_win, A_BOLD | A_ITALIC);
    }

    // Print menu
    for (int i = 0; i < NO_CHOICES; i++)
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
    int which_board = x / 3;
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

    const int GRID_HEIGHT = 7;
    const int GRID_WIDTH  = 13;

    // Print grid
    for (int i = 0; i < GRID_HEIGHT; i++)
    {
        mvwprintw(board_win, i, 0, "%s", grid[i]);
    }

    // Print elements
    int x, y;
    x = y = 0;

    for (int i = 1; i < GRID_HEIGHT; i += 2)
    {
        x = 0;

        for (int j = 2; j < GRID_WIDTH; j += 4)
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
                    " - Undo",
                    " - Redo",
                    " - Save game",
                    " - Load game",
                    " - Playing stats",
                    " - Quit"};

    char *highlighted_menu[] = {"     -> Restart                         ",
                                "     -> Continue                        ",
                                "     -> Undo                            ",
                                "     -> Redo                            ",
                                "     -> Save game                       ",
                                "     -> Load game                       ",
                                "     -> Playing stats                   ",
                                "     -> Quit                            "};
    
    const int NO_MENU_CHOICES = 8;

    // Print borders & tag
    wclear(menu_win);
    box(menu_win, 0, 0);
    mvwprintw(menu_win, 0, 1, "%s", tag);

    // Print menu with choice highlighted
    for (int i = 0; i < NO_MENU_CHOICES; i++)
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
    char *two_player_status[] = {"Player 1 to play",
                                 "Player 2 to play"};
    char *computer_status[]   = {"Thinking ...",
                                 "Your turn"};

    // Clear status window & print borders
    wclear(status_win);
    box(status_win, 0, 0);

    // Print tag
    mvwprintw(status_win, 0, 1, "%s", tag);

    // Printing position
    const int X = 4;
    const int Y = 1;

    // Print turn in computer mode
    if (which_mode == COMPU_MODE)
    {
        // Computer turn
        if (turn == 1)
        {
            mvwprintw(status_win, Y, X, "%s", computer_status[0]);
        }
        // User turn 
        else if (turn == -1)
        {
            mvwprintw(status_win, Y, X, "%s", computer_status[1]);
        }
    }
    // Print turn in two player mode
    else if (which_mode == HUMAN_MODE)
    {
        // Player 1 turn 
        if (turn == 1)
        {
            mvwprintw(status_win, Y, X, "%s", two_player_status[0]);
        }
        // Player 2 turn 
        else if (turn == -1)
        {
            mvwprintw(status_win, Y, X, "%s", two_player_status[1]);
        }
    }

    wrefresh(status_win);
}

// Print game stats 
void print_stats(int engine_games[2], int two_user_games[2])
{
    // Clear main window
    wclear(main_win);
    box(main_win, 0 , 0);
    wrefresh(main_win);

    // Calculate percentages
    int t_engine_games, t_user_games;
    float comp_wins, comp_loses, p1_wins, p2_wins;

    comp_wins = comp_loses = p1_wins = p2_wins = 0;   

    t_engine_games = engine_games[0] + engine_games[1];
    t_user_games = two_user_games[0] + two_user_games[1];

    if (t_engine_games)
    {
        comp_wins  = (engine_games[0] * 100.0) / t_engine_games;
        comp_loses = (engine_games[1] * 100.0) / t_engine_games;
    }

    if (t_user_games)
    {
        p1_wins = (two_user_games[0] * 100.0) / t_user_games;
        p2_wins = (two_user_games[1] * 100.0) / t_user_games;
    }

    // Print stats
    const int STATS_WIN_WIDTH  = 58;
    const int STATS_WIN_HEIGHT = 10;

    mvwprintw(stats_win, 0, 0, "| TOTAL GAMES      : %3i", t_user_games + t_engine_games);
    mvwprintw(stats_win, 1, 0, " --------------------------------------------------------");
    mvwprintw(stats_win, 2, 0, "| vs Computer      : %3i  | Wins          : %3i   | %%%2.2f", t_engine_games, engine_games[0], comp_wins );
    mvwprintw(stats_win, 3, 0, "|                         | Loses         : %3i   | %%%2.2f"                , engine_games[1], comp_loses);
    mvwprintw(stats_win, 4, 0, "|");
    mvwprintw(stats_win, 5, 0, "| Two Player games : %3i  | Player 1 wins : %3i   | %%%2.2f", t_user_games, two_user_games[0], p1_wins);
    mvwprintw(stats_win, 6, 0, "|                         | Player 2 wins : %3i   | %%%2.2f"              , two_user_games[1], p2_wins);

    char *prompt = "PRESS ANY KEY TO RETURN";
    mvwprintw(stats_win, STATS_WIN_HEIGHT - 1, (STATS_WIN_WIDTH - strlen(prompt)) / 2, "%s", prompt);

    wrefresh(stats_win);

    resize_or_quit(getch());

    wclear(stats_win);
    wrefresh(stats_win);
}

// Print game ending message
void print_end_msg(int who_won)
{
    char *win_msg[]  = {"Congratulations",
                        "PLAYER ?",
                        "You won"};
    char *lose_msg[] = {"You lost",
                        "Better luck next time"};

    wclear(endgame_win);
    wclear(main_win);

    box(main_win, 0, 0);
    wrefresh(endgame_win);
    wrefresh(main_win);

    // Get window size & printing position
    const int COLS = getmaxx(endgame_win);
    int x1, x2;

    // Computer mode 
    if (which_mode == COMPU_MODE)
    {
        // User won
        if (who_won == -1)
        {
            x1 = (COLS - strlen(win_msg[0])) / 2;
            x2 = (COLS - strlen(win_msg[2])) / 2;

            mvwprintw(endgame_win, 0, x1, "%s", win_msg[0]);
            mvwprintw(endgame_win, 1, x2, "%s", win_msg[2]);
        }
        // Computer won
        else if (who_won == 1)
        {
            x1 = (COLS - strlen(lose_msg[0])) / 2;
            x2 = (COLS - strlen(lose_msg[1])) / 2;

            mvwprintw(endgame_win, 0, x1, "%s", lose_msg[0]);
            mvwprintw(endgame_win, 1, x2, "%s", lose_msg[1]);
        }
    }
    // Two user mode
    else if (which_mode == HUMAN_MODE)
    {
        // Find winner -> player 1 or 2 
        char *winner = (who_won == 1) ? "PLAYER 1" : "PLAYER 2";

        x1 = (COLS - strlen(win_msg[0])) / 2;
        x2 = (COLS - strlen(win_msg[1])) / 2;

        mvwprintw(endgame_win, 0, x1, "%s", win_msg[0]);
        mvwprintw(endgame_win, 1, x2, "%s", winner);
    }

    wrefresh(endgame_win);
}

// Print error messages
// which_win: indicates where to print error message -> 2: standard screen, 1: main_win, 0: error_win
void print_error(int error_num, int which_win)
{
    // Error messages
    char *tag = "[ Error: ";
    char *error_msgs[] = {"dead board ]",                       // 0
                          "move already played ]",              // 1
                          "no choice ]",                        // 2
                          "invalid key ]",                      // 3 
                          "board border ]",                     // 4
                          "aleardy at oldest change ]",         // 5
                          "already at newest change ]",         // 6
                          "Please increase terminal size ]",    // 7
                          "couldn't save game ]",               // 8
                          "file doesn't exist ]",               // 9
                          "loading failed ]"};                  // 10

    // Get window size & printing position
    int rows, cols, y, x;

    // Print error message in error window
    if (!which_win) 
    {
        getmaxyx(error_win, rows, cols);

        x = (cols - strlen(error_msgs[error_num]) - strlen(tag)) / 2;
        x = (x >= 0) ? x : 0;
        y = 0;

        // Print error
        wclear(error_win);
        mvwprintw(error_win, y, x, "%s%s", tag, error_msgs[error_num]);
        wrefresh(error_win);
    }
    // Print error message in main window
    else if (which_win == 1)
    {
        getmaxyx(main_win, rows, cols);

        x = (cols - strlen(error_msgs[error_num]) - strlen(tag)) / 2;
        x = (x >= 0) ? x : 0;
        y = rows / 2;
        
        // Print error
        wclear(main_win);
        box(main_win, 0, 0);
        
        mvwprintw(main_win, y, x, "%s%s", tag, error_msgs[error_num]);
        wrefresh(main_win);
    }
    // Print error message in standard screen
    else if (which_win == 2)
    {
        getmaxyx(stdscr, rows, cols);

        x = (cols - strlen(error_msgs[error_num]) - strlen(tag)) / 2;
        x = (x >= 0) ? x : 0;
        y = rows / 2;
        
        // Print error
        clear();
        mvprintw(y, x, "%s%s", tag, error_msgs[error_num]);
        refresh();
    }
}

