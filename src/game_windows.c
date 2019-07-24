/* Windows used in game */
#include <ncurses.h>
#include <string.h>

/* DEFINITIONS */
#define LOGO_WIDTH 62
#define LOGO_HEIGHT 5

#define HUMAN_MODE 0
#define COMPU_MODE 1

#define BOARDS_WIN 0 
#define MENU_WIN   1 

#define NO_BOARDS 3
#define NO_MENU_CHOICES 7

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

void adjust_windows();
void is_resized(int ch);

void print_logo();
void print_instructions();

void print_side_menu(int which_win);
void print_boards(int x, int y);
void print_board(int board[3][3], WINDOW *board_win);
void print_menu(int which);
void print_status(int turn);
void print_stats(int no_games[2], int no_wins[2], int no_loses[2]);
void print_end_msg(int who_won);
void print_error(int error_num, int which_win);

// Create windows used in game
// Returns 1: windows created, 0: otherwise
int create_windows()
{
    // Minimum terminal size to play game
    int m_width, m_height;
    m_width  = 96;
    m_height = 24;

    // Get screen size
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    if (rows >= m_height && cols >= m_width)
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
        height = 11;
        width = 50;
        y = (rows - height - 9) / 2;
        x = (cols - width) / 2;

        menu_win = newwin(height, width, y, x);

        // Create error window -> inside main window
        height = 3;
        width = 34;
        y = rows - 9 - 4;
        x = (cols - 25) / 2;

        error_win = newwin(height, width, y, x);

        // Create status (turn) window -> inside main window
        height = 3;
        width = 24;
        y = rows - 9 - 4;
        x = 2;

        status_win = newwin(height, width, y, x);

        // Create stats window -> inside main window
        height = 10;
        width = 90;
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
    }while ((ch = getch()) == KEY_RESIZE);

    // Re-print windows
    box(main_win, 0, 0);
    wrefresh(main_win);

    print_logo();
    print_instructions();
}

// If resizing was detected
void is_resized(int ch)
{
    if (ch == KEY_RESIZE)
    {
        adjust_windows();
    }
}

/* Printing functions */

/* Static windows */
void print_logo()
{
    // Logo & author
    // Logo is represented using an integer array where -> 0: ' ', 1: block, 2: |, 4: -
    char *author = "@sudo-sturbia";
    int logo_arr[LOGO_HEIGHT][LOGO_WIDTH] = {{1, 1, 1, 2, 0, 0, 0, 1, 1, 2, 0, 1, 1, 1, 1, 1, 1, 2, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 1, 1,
                                              1, 1, 1, 2, 0, 1, 1, 2, 0, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 1, 1, 1, 1, 1, 1, 2, 0},
                                             {1, 1, 1, 1, 2, 0, 0, 1, 1, 2, 1, 1, 2, 4, 4, 4, 1, 1, 2, 0, 4, 4, 1, 1, 2, 4, 4, 0, 1, 1, 2,
                                              4, 4, 1, 1, 2, 1, 1, 2, 0, 1, 1, 2, 0, 0, 4, 4, 1, 1, 4, 4, 4, 0, 1, 1, 2, 4, 4, 4, 1, 1, 2},
                                             {1, 1, 2, 1, 1, 2, 0, 1, 1, 2, 1, 1, 2, 0, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 1,
                                              1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 2},
                                             {1, 1, 2, 0, 1, 1, 2, 1, 1, 2, 1, 1, 2, 0, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 2,
                                              4, 4, 1, 1, 2, 1, 1, 2, 4, 1, 1, 2, 0, 0, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 2},
                                             {1, 1, 2, 0, 0, 1, 1, 1, 1, 2, 0, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 2,
                                              0, 0, 1, 1, 2, 1, 1, 2, 0, 0, 1, 1, 2, 0, 0, 0, 1, 1, 2, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 0}
                                             };

    // Logo window size
    int win_rows, win_cols;
    win_rows = 9;
    win_cols = 65;

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
                case 4:
                    mvwaddch(logo_win, i + 2, j + 2, '-');
                    break;
            }
        }
    }

    // Print author
    int y, x;
    y = win_rows - 2;
    x = win_cols - strlen(author) - 5;

    mvwprintw(logo_win, y, x, "%s", author);

    wrefresh(logo_win);
}

// Print game instructions 
void print_instructions()
{
    char *instructions[] = {" INSTRUCTIONS:",
                            "- To navigate:",
                            "                   ^",
                            "           < h  j  k  l >",
                            "                v",
                            "\n",
                            "- For side menu:  s",
                            "- To quit:        q"};

    // Get window size & printing position
    int win_cols, x;
    win_cols = getmaxx(instructions_win);

    x = win_cols / 8;
    x = (x > 0) ? x : 1;           // x must be higher than 0

    // Print window tag
    mvwprintw(instructions_win, 0, 1, "%s", instructions[0]);

    // Print instructions
    for (int i = 1; i <= 7; i++)
    {
        mvwprintw(instructions_win, i, x, "%s", instructions[i]);
    }

    // Print borders
    box(instructions_win, 0, 0);

    wrefresh(instructions_win);
}

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
                    " - Playing stats",
                    " - Save game",
                    " - Quit"};

    char *highlighted_menu[] = {"     -> Restart                         ",
                                "     -> Continue                        ",
                                "     -> Undo last move                  ",
                                "     -> Redo last move                  ",
                                "     -> Playing stats                   ",
                                "     -> Save game                       ",
                                "     -> Quit                            "};
    
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

// Print game stats 
void print_stats(int no_games[2], int no_wins[2], int no_loses[2])
{
    // Clear main window
    wclear(main_win);
    box(main_win, 0 , 0);
    wrefresh(main_win);

    // Calculate percentages
    float comp_wins, comp_loses, player1_wins, player2_wins;
    if (no_games[0] != 0)
    {
        comp_wins = (no_wins[0] * 100.0) / no_games[0];
        comp_loses = (no_loses[0] * 100.0) / no_games[0];
    }
    else
    {
        comp_wins = comp_loses = 0;
    }

    if (no_games[1] != 0)
    {
        player1_wins = (no_wins[1] * 100.0) / no_games[1];
        player2_wins = (no_loses[1] * 100.0) / no_games[1];
    }
    else
    {
        player1_wins = player2_wins = 0;
    }

    // Print stats
    int rows, cols;
    rows = 10; 
    cols = 90;

    mvwprintw(stats_win, 1, 0, "| No. of games | %3i", no_games[0] + no_games[1]);

    mvwprintw(stats_win, 3, 0, "| vs COMPUTER          |  %3i                  | TWO PLAYER           | %3i", no_games[0], no_games[1]);
    mvwprintw(stats_win, 4, 0, "| No. of wins                                  | PLAYER 1 wins ");
    mvwprintw(stats_win, 5, 0, " --------------->  %3i | percentage: %%%5.2f     --------------->  %3i | percentage: %%%5.2f", no_wins[0], comp_wins, no_wins[1], player1_wins);
    mvwprintw(stats_win, 6, 0, "| No. of loses                                 | PLAYER 2 wins ");
    mvwprintw(stats_win, 7, 0, " --------------->  %3i | percentage: %%%5.2f     --------------->  %3i | percentage: %%%5.2f", no_loses[0], comp_loses, no_loses[1], player2_wins);

    mvwprintw(stats_win, rows - 1, (cols - 23) / 2, "PRESS ANY KEY TO RETURN");

    wrefresh(stats_win);

    is_resized(getch());
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
    int cols, x1, x2;
    cols = getmaxx(endgame_win);

    // Computer mode 
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
    // Two user mode
    else if (which_mode == HUMAN_MODE)
    {
        // Find winner 
        char *winner = (who_won == 1) ? "PLAYER 1" : "PLAYER 2";

        x1 = (cols - strlen(win_msg[0])) / 2;
        x2 = (cols - strlen(win_msg[1])) / 2;

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

    // Print in main window
    if (which_win == 1)
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
    // Print in error window
    else if (!which_win) 
    {
        getmaxyx(error_win, rows, cols);

        x = (cols - strlen(error_msgs[error_num]) - strlen(tag)) / 2;
        x = (x >= 0) ? x : 0;
        y = 1;

        // Print error
        wclear(error_win);
        mvwprintw(error_win, y, x, "%s%s", tag, error_msgs[error_num]);
        wrefresh(error_win);
    }
    // Print in standard screen
    else
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

