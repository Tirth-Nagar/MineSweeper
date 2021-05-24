#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <curses.h>
#include "mines.h"


// (x, y)-based access to game cells:
#define cell_xy(X, Y) (game.cells[(X) + (Y) * game.nx])


/* Global program state */

Board board;
Game game;

size_t cursorX;
size_t cursorY;

WINDOW *boardWindow = NULL;
WINDOW *headerWindow = NULL;

int gameInfo[3] = {9, 9, 10};

int mainSetup(void)
{

	int start_y = 0, start_x = 0;

	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(4, COLOR_CYAN, COLOR_BLACK);
	init_pair(5, COLOR_BLACK, COLOR_YELLOW);
	init_pair(6, COLOR_WHITE, COLOR_BLACK);
	init_pair(7, COLOR_RED, COLOR_WHITE);

	int cols = 0, rows = 0;
	getmaxyx(stdscr, rows, cols);

	WINDOW *win = newwin(rows, cols, start_y, start_x);
	refresh();

	FILE *p = fopen("logo.txt", "r");
	char str[200];
	int i = 1;

	while (fgets(str, 200, p) != NULL)
	{
		attron(COLOR_PAIR(1));
		mvprintw(i++, COLS / 2 - 50, "%s", str);
		attroff(COLOR_PAIR(1));
		wrefresh(win);
	}

	fclose(p);

	return 0;
}

int playGame();

int menuSetup(void)
{

	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(4, COLOR_CYAN, COLOR_BLACK);
	init_pair(5, COLOR_BLACK, COLOR_YELLOW);
	init_pair(6, COLOR_WHITE, COLOR_BLACK);
	init_pair(7, COLOR_RED, COLOR_WHITE);

	int cols = 0, rows = 0;
	getmaxyx(stdscr, rows, cols);

	int start_y = 8, start_x = 0;

	WINDOW *opt = newwin(rows - start_y, cols, start_y, start_x);
	refresh();

	keypad(opt, true);

	getmaxyx(opt, rows, cols);

	box(opt, 0, 0);

	wrefresh(opt);

	char *options1[4] = {"New game", "Settings", "About", "Quit"};
	int charNum[4] = {strlen(options1[0]), strlen(options1[1]), strlen(options1[2]), strlen(options1[3])};
	int choice = 0, highlight = 0;

	while (1)
	{
		for (int i = 0; i < 4; i++)
		{
			if (i == highlight)
			{
				wattron(opt, A_REVERSE);
				wattron(opt, COLOR_PAIR(1));
			}
			mvwprintw(opt, rows / 2 + i, (cols - charNum[i]) / 2, "%s", options1[i]);
			wrefresh(opt);
			wattroff(opt, COLOR_PAIR(1));
			wattroff(opt, A_REVERSE);
		}
		choice = wgetch(opt);

		switch (choice)
		{
		case KEY_UP:
			highlight--;
			if (highlight == -1)
			{
				highlight = 3;
			}
			break;
		case KEY_DOWN:
			highlight++;
			if (highlight == 4)
			{
				highlight = 0;
			}
			break;
		default:
			break;
		}

		if (choice == 10)
		{
			break;
		}
	}

	wclear(opt);
	refresh();

	box(opt, 0, 0);
	wrefresh(opt);

	if (highlight == 0)//Play Game
	{
		playGame();
	}

	else if (highlight == 1) //Settings
	{
		char *settings[3] = {"Number of Rows", "Number of Columns", "Number of Mines"};
		int charNum[3] = {strlen(settings[0]) + 2, strlen(settings[1]) + 2, strlen(settings[2]) + 2};
		choice = 0, highlight = 0;

		while (1)
		{
			for (int i = 0; i < 3; i++)
			{
				if (i == highlight)
				{
					wattron(opt, A_REVERSE);
					wattron(opt, COLOR_PAIR(1));
				}
				mvwprintw(opt, rows / 2 + i, (cols - charNum[i]) / 2, "%s: %i", settings[i], gameInfo[i]);
				wrefresh(opt);
				wattroff(opt, COLOR_PAIR(1));
				wattroff(opt, A_REVERSE);
			}

			wattron(opt, A_STANDOUT);
			mvwprintw(opt, rows / 2 + 5, (cols - 22) / 2, " Press 'X' To Go Back ");
			wattroff(opt, A_STANDOUT);
			wrefresh(opt);

			choice = wgetch(opt);

			switch (choice)
			{
			case KEY_UP:
				highlight--;
				if (highlight == -1)
				{
					highlight = 2;
				}
				break;
			case KEY_DOWN:
				highlight++;
				if (highlight == 3)
				{
					highlight = 0;
				}
				break;
			case 'x':
				wclear(stdscr);
				mainSetup();
				menuSetup();
			default:
				break;
			}

			if (choice == 10)
			{
				int number = 0;
				mvwprintw(opt, rows / 2 + highlight, (cols - charNum[highlight]) / 2, "%s: ", settings[highlight]);
				wrefresh(opt);
				wscanw(opt, "%i", &number);
				if (highlight == 0 || highlight == 1)
				{
					if (number > 9 && number <= 30)
					{
						gameInfo[highlight] = number;
						mvwprintw(opt, rows / 2 + highlight, (cols - charNum[highlight]) / 2, "%s: %i", settings[highlight], gameInfo[highlight]);
						wrefresh(opt);
					}
					else
					{
						continue;
					}
				}
				if (highlight == 2)
				{
					if (number <= ((gameInfo[0] * gameInfo[1]) - 1))
					{
						gameInfo[highlight] = number;
						mvwprintw(opt, rows / 2 + highlight, (cols - charNum[highlight]) / 2, "%s: %i", settings[highlight], gameInfo[highlight]);
						wrefresh(opt);
					}
				}
				if (number == 'x' || number == 'X')
				{
					wclear(stdscr);
					mainSetup();
					menuSetup();
				}
			}
		}
	}

	else if (highlight == 2) //About
	{
		wattron(opt, A_UNDERLINE);
		mvwprintw(opt, rows / 2, (cols - 38) / 2, "Hello welcome to the minesweeper clone");
		mvwprintw(opt, rows / 2 + 1, (cols - 36) / 2, "made by Tirth Nagar using the Curses");
		mvwprintw(opt, rows / 2 + 2, (cols - 40) / 2, "Library the rules are the same as normal");
		mvwprintw(opt, rows / 2 + 3, (cols - 27) / 2, "Minesweeper so Go Have Fun!");
		wattroff(opt, A_UNDERLINE);

		wattron(opt, A_STANDOUT);
		mvwprintw(opt, rows / 2 + 5, (cols - 22) / 2, " Press 'X' To Go Back ");
		wattroff(opt, A_STANDOUT);
		wrefresh(opt);

		char ch;
		while (ch != 'X' || ch != 'x')
		{
			ch = getch();

			if (ch == 'x' || ch == 'X')
			{
				wclear(stdscr);
				mainSetup();
				menuSetup();
			}
		}
	}

	else if (highlight == 3) //Quit
	{
		echo();
		refresh();
		endwin();
		erase();
		exit(0);
	}
	return 0;
}

int playGame(){

	void move_cursor_left(bool skipping) {
    while (cursorX > 0) {
        cursorX--;
        // When skipping, find the next non-0 cell in the direction:
        if (!skipping || cell_xy(cursorX, cursorY) != STATUS_ZERO) break;
    }
}

void move_cursor_right(bool skipping) {
    while (cursorX < game.nx - 1) {
        cursorX++;
        if (!skipping || cell_xy(cursorX, cursorY) != STATUS_ZERO) break;
    }
}

void move_cursor_up(bool skipping) {
    while (cursorY > 0) {
        cursorY--;
        if (!skipping || cell_xy(cursorX, cursorY) != STATUS_ZERO) break;
    }
}

void move_cursor_down(bool skipping) {
    while (cursorY < game.ny - 1) {
        cursorY++;
        if (!skipping || cell_xy(cursorX, cursorY) != STATUS_ZERO) break;
    }
}


// An extended action (uncover all non-flagged adjacent cells) is available for
// cells with 1-8 markers:
bool extended_action_available() {
    const Status status = cell_xy(cursorX, cursorY);
    return status > STATUS_ZERO && status != STATUS_MINE;
}



/* Interface */

void draw_layout() {
    // Clean up old windows:
    if (boardWindow != NULL) delwin(boardWindow);
    if (headerWindow != NULL) delwin(headerWindow);
    wclear(stdscr);
    // Determine available screen size:
    int termW, termH;
    getmaxyx(stdscr, termH, termW);
    // Determine required screen size:
    const int boardW = (int) (4*game.nx + 1);
    const int boardH = (int) (2*game.ny + 1);
    const int headerH = 3;
    // Terminal is not big enough to contain board. Display a message
    // containing the required size:
    if (termH < boardH + headerH || termW < boardW) {
        wattrset(stdscr, A_BOLD);
        mvwprintw(stdscr, 0, 0, "required terminal size: %d x %d\n"
                                "available terminal size: %d x %d", boardW, boardH + headerH, termW, termH);
        boardWindow = NULL;
        headerWindow = NULL;
    // Create (centered) windows for the header and game board:
    } else {
        const int offsetX = (termW - boardW) / 2;
        const int offsetY = (termH - boardH - headerH) / 2;
        boardWindow = newwin(boardH, boardW, offsetY + headerH, offsetX);
        headerWindow = newwin(headerH, boardW, offsetY, offsetX);
    }
    // Fill the background:
    wbkgd(stdscr, COLOR_PAIR(1));
    wrefresh(stdscr);
    // Board and header have to be refreshed separately
}

void draw_board() {
    if (boardWindow == NULL) return;
    werase(boardWindow);
    for (size_t y = 0; y < game.ny; y++) {
        // Every second row in y-direction
        const size_t screenY = 1 + 2*y;
        for (size_t x = 0; x < game.nx; x++) {
            // Every fourth row in x-direction
            const size_t screenX = 2 + 4*x;
            Status cellStatus = cell_xy(x, y);
            // Output symbol:
            chtype ch;
            switch (cellStatus) {
                case STATUS_UNKNOWN:
                    ch = ACS_BULLET;
                    break;
                case STATUS_MINE:
                    ch = '#';
                    break;
                case STATUS_FLAG:
                    ch = 'X';
                    break;
                case STATUS_ZERO:
                    ch = ' ';
                    break;
                case STATUS_ONE:
                    ch = '1';
                    break;
                case STATUS_TWO:
                    ch = '2';
                    break;
                case STATUS_THREE:
                    ch = '3';
                    break;
                case STATUS_FOUR:
                    ch = '4';
                    break;
                case STATUS_FIVE:
                    ch = '5';
                    break;
                case STATUS_SIX:
                    ch = '6';
                    break;
                case STATUS_SEVEN:
                    ch = '7';
                    break;
                case STATUS_EIGHT:
                    ch = '8';
                    break;
            }
            // Apply style depending on cell status:
            wattrset(boardWindow, COLOR_PAIR(10 + cellStatus));
            mvwaddch(boardWindow, screenY, screenX, ch);
        }
    }
    // Screen position of cursor:
    const size_t x = 2 + 4*cursorX;
    const size_t y = 1 + 2*cursorY;
    // Draw inner cursor:
    wattrset(boardWindow, COLOR_PAIR(20));
    mvwaddch(boardWindow, y + 1, x + 2, ACS_LRCORNER);
    mvwaddch(boardWindow, y + 1, x - 2, ACS_LLCORNER);
    mvwaddch(boardWindow, y - 1, x + 2, ACS_URCORNER);
    mvwaddch(boardWindow, y - 1, x - 2, ACS_ULCORNER);
    // Draw outer (3x3) cursor only if the extended action is available:
    if (extended_action_available()) {
        wattrset(boardWindow, COLOR_PAIR(21));
        if (cursorX < game.nx - 1 && cursorY < game.ny - 1) mvwaddch(boardWindow, y + 3, x + 6, ACS_LRCORNER);
        if (cursorX > 0 && cursorY < game.ny - 1) mvwaddch(boardWindow, y + 3, x - 6, ACS_LLCORNER);
        if (cursorX < game.nx - 1 && cursorY > 0) mvwaddch(boardWindow, y - 3, x + 6, ACS_URCORNER);
        if (cursorX > 0 && cursorY > 0) mvwaddch(boardWindow, y - 3, x - 6, ACS_ULCORNER);
    }
    // Apply changes:
    wrefresh(boardWindow);
}





}












int main()
{
	initscr();
	noecho();
	cbreak();
	curs_set(0);

	mainSetup();
	
	menuSetup();

	// Initialize interface:
    draw_layout();

	while (true) {
        // Update game board:
        draw_board();
        // Obtain user input:
        const int key = getch();
        // Exit program on q or ^D
        if (key == 'q' || key == ('d' & 0x1F)) break;
        // Actions
        switch (key) {
            case 'k':
            case 'K':
            case KEY_UP:
                move_cursor_up(key == 'K');
                break;
            case 'j':
            case 'J':
            case KEY_DOWN:
                move_cursor_down(key == 'J');
                break;
            case 'h':
            case 'H':
            case KEY_LEFT:
                move_cursor_left(key == 'H');
                break;
            case 'l':
            case 'L':
            case KEY_RIGHT:
                move_cursor_right(key == 'L');
                break;
            case 'f':
            case 'm':
                toggle_flag(&game, cursorX, cursorY);
                break;
            case ' ':
                // The first move is always safe
                if (game.nopen == game.nx * game.ny) {
                    make_safe(&board, cursorX, cursorY);
                }
                // Normal or extended action?
                if (extended_action_available()) {
                    uncover_adjacent(&game, cursorX, cursorY);
                } else {
                    uncover(&game, cursorX, cursorY);
                }
                break;
        }
        
        // Game has ended:
        if (game.state != UNDECIDED) {
            draw_board();
            draw_header();
            getch(); // any key to quit
            break;
        }

    }

	getch();

	endwin();

	return 0;
}
