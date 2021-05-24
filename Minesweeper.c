#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <curses.h>

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

	if (highlight == 0)
	{
		NULL; //New Game
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
int main()
{

	initscr();
	noecho();
	cbreak();
	curs_set(0);

	mainSetup();
	menuSetup();

	getch();

	endwin();

	return 0;
}
