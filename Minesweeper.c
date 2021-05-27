/****************************************************************************************/
/*																						*/
/* Author: Tirth Nagar 																	*/
/* Program Name: MineSweeper 														    */
/* Description: This program is a game where I cloned the infamous minesweeper game		*/
/* in which you can move around wiht WASD or arrow keys and flag mines and navigate		*/
/* through the minefield the program is recursive and you can increase or decrease the  */
/* difficulty through the settings. Enjoy!                                  			*/
/* Date: Thursday May 27, 2021															*/
/* 																						*/
/****************************************************************************************/

//Terminal Size 138x41 is reccommed for 9x9

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <curses.h>
#include <errno.h>
#include <time.h>

// Define Macros for Constant values to be used throughout the code
#define DEFAULT_WIDTH 9
#define DEFAULT_HEIGHT 9
#define CW 5
#define CH 3
#define DEFAULT_MINE 10
#define DEFAULT_C 6
#define MINE '*'
#define FLAG 'F'
#define ZERO '0'
#define SHOW_MASK 0x80
#define VALUE_MASK 0x3f
#define FLAG_MASK 0x40

// Create Function Prototypes
void printBoard(int physicalH, int physicalW);
void initGame();
void resetTerminal();
void initKeypad();
bool testMine();
bool showSquare(int currY, int currX);
void flagMine();
void showAll();
void gameWin();
void gameOver();
void addChar(int y, int x, chtype c);
void printInfo();
void updateInfo();
void initColorPairs();
int mainSetup();
int menuSetup();
int playGame();

// Create Global Variables to be used throughout the program
int W = DEFAULT_WIDTH, H = DEFAULT_HEIGHT, N = DEFAULT_MINE;
int X = 0, Y = 0, STEP_X = CW - 1, STEP_Y = CH - 1;
WINDOW *board_win;
unsigned char *board_arry;
unsigned char *temp_arry;
unsigned int tempSize;
char *info;
char *infoFormat = "There is %d mines remaining on the field";
char *control = "MOVE: [AWDS or arrows] PROBE: [SPACE] FLAG: F ";
int REMAIN_N = 0, CORRECT_N = 0, GAME_END_PRESS = 0;
bool GAME_END = false, COL_SUPP = false;

// Array for user inputted settings
int gameInfo[3] = {DEFAULT_HEIGHT, DEFAULT_WIDTH, DEFAULT_MINE};

// Main Function
int main()
{

	initscr();	 // Initialize Curses
	noecho();	 // No displaying typed values
	cbreak();	 // Don’t wait for return key for input
	curs_set(0); // Do not display blinking cursor

	mainSetup(); // Execute the mainSetup(); function
	menuSetup(); // Execute the menuSetup(); function

	return 0; // Before terminating program return 0
}

// Create the logo header
int mainSetup(void)
{

	initColorPairs(); // Execute the initColorPairs(); function

	int cols = 0, rows = 0;
	getmaxyx(stdscr, rows, cols); // Figure out the size of the terminal in rows and columns

	WINDOW *win = newwin(rows, cols, Y, X); // Create a window the same size as the terminal
	refresh();
	wrefresh(win);

	FILE *p = fopen("logo.txt", "r"); // Open the logo file which contains the ASCII Minesweeper logo and print it
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

// Setup the menu and all the functions
int menuSetup(void)
{

	initColorPairs(); // Execute the initColorPairs(); function

	int cols = 0, rows = 0;
	getmaxyx(stdscr, rows, cols); // Figure out the size of the terminal in rows and columns

	int start_y = 8, start_x = 0;

	WINDOW *opt = newwin(rows - start_y, cols, start_y, start_x); // Create a window the same size as the terminal in width subtracting the amount of lines the logo takes
	refresh();

	keypad(opt, true); // Initialize keypad

	getmaxyx(opt, rows, cols); // Figure out the size of the terminal in rows and columns

	box(opt, 0, 0); // Draw a box/border around the  opt window

	wrefresh(opt);

	char *options1[4] = {"New game", "Settings", "About", "Quit"};										   // Create an array to store options
	int charNum[4] = {strlen(options1[0]), strlen(options1[1]), strlen(options1[2]), strlen(options1[3])}; // Create an array to store the length of each option
	int choice = 0, highlight = 0;

	while (1)
	{
		for (int i = 0; i < 4; i++) // Print all the options
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

		choice = wgetch(opt); // Initialize controls for the settings menu

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

	wclear(opt); // Clear the opt window
	refresh();

	box(opt, 0, 0); // Redraw the box/border around the opt window
	wrefresh(opt);

	if (highlight == 0) // If New Game option selected do this
	{
		curs_set(1); // Show cursor
		playGame();
	}

	else if (highlight == 1) // If Settings option selcted do this
	{
		char *settings[3] = {"Number of Rows", "Number of Columns", "Number of Mines"};				  // Create an array to store options
		int charNum[3] = {strlen(settings[0]) + 2, strlen(settings[1]) + 2, strlen(settings[2]) + 2}; // Create an array to store the length of each option + 2 chars for the numbers
		choice = 0, highlight = 0;

		while (1)
		{
			for (int i = 0; i < 3; i++) // Print all the options
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

			wattron(opt, A_STANDOUT); // Print footer/ bottom header to inform the user that this is also a valid option
			mvwprintw(opt, rows / 2 + 5, (cols - 22) / 2, " Press 'X' To Go Back ");
			wattroff(opt, A_STANDOUT);
			wrefresh(opt);

			choice = wgetch(opt);

			switch (choice) // Initialize controls for the settings menu
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

			if (choice == 10) // If the enter key is pressed once selected an option do this
			{
				int number = 0;

				mvwprintw(opt, rows / 2 + highlight, (cols - charNum[highlight]) / 2, "%s: ", settings[highlight]); // Print option selected without the number
				wrefresh(opt);
				wscanw(opt, "%i", &number); // Get user to input the number

				if (highlight == 0 || highlight == 1) // If option one or two is selected 
				{
					if (number >= 9 && number <= 30)  // Ensure that the number entered is between 9 and 30 and do this
					{
						gameInfo[highlight] = number; // Set the option values to the number entered
						
						wclear(opt); // Clear opt window
						refresh();

						box(opt, 0, 0); // Create box/border around opt window
						wrefresh(opt);

						for (int i = 0; i < 3; i++) // Print all the options again with the inputted number
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

						wattron(opt, A_STANDOUT); // Print footer/ bottom header to inform the user that this is also a valid option
						mvwprintw(opt, rows / 2 + 5, (cols - 22) / 2, " Press 'X' To Go Back ");
						wattroff(opt, A_STANDOUT);
						wrefresh(opt);
					}
					else
					{
						continue;
					}
				}
				if (highlight == 2) // If option three is selected do this
				{
					if (number <= ((gameInfo[0] * gameInfo[1]) - 1) && number > 0) // Ensure the number entered is between 0 and 1 less than the area of the grid
					{
						gameInfo[highlight] = number;
						
						wclear(opt); // Clear opt window
						refresh();

						box(opt, 0, 0); // Create box/border around opt window
						wrefresh(opt);

						for (int i = 0; i < 3; i++) // Print all the options again with the inputted number
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
						wattron(opt, A_STANDOUT); // Print footer/ bottom header to inform the user that this is also a valid option
						mvwprintw(opt, rows / 2 + 5, (cols - 22) / 2, " Press 'X' To Go Back ");
						wattroff(opt, A_STANDOUT);
						wrefresh(opt);
					}
				}
				if (number == 'x' || number == 'X') // If X is pressed go back to main menu
				{
					wclear(stdscr);
					mainSetup();
					menuSetup();
				}
			}
		}
	}

	else if (highlight == 2) // If About option is selected do this:
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

		char ch; // If X is pressed go back to main menu
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

	else if (highlight == 3) // If Quit option is selected do this
	{
		echo();
		refresh();
		endwin();
		erase();
		exit(0);
	}

	return 0;
}

// Setup and play the game
int playGame()
{
	mainSetup();

	int rows, cols;

	getmaxyx(stdscr, rows, cols);

	H = gameInfo[0];
	W = gameInfo[1];
	N = gameInfo[2];

	board_arry = calloc(H * W, sizeof(unsigned char)); // Allocate memory to the arrays
	tempSize = H * W * 2;
	temp_arry = calloc(tempSize, sizeof(unsigned char));
	info = calloc(120, sizeof(char));

	savetty(); // Save teminal Settings

	initGame(board_arry);
	REMAIN_N = N;

	int physicalW = W * STEP_X + 1;
	int physicalH = H * STEP_Y + 1;

	board_win = subwin(stdscr, physicalH, physicalW, 8, cols / 2.75); // Create the board window

	initColorPairs();

	attron(A_STANDOUT);
	mvprintw((W * STEP_X + 1) - 4, (cols - 22) / 2, " Press 'X' To Go Back ");
	attroff(A_STANDOUT);
	wrefresh(board_win);

	printBoard(physicalH, physicalW); // Call and run the printBoard function with necessary

	updateInfo();
	mvprintw((W * STEP_X + 1) - 6, (cols - 45) / 2, control);

	wmove(board_win, 1, 2); // Move cursor to following coordinates in the window
	X = 0;
	Y = 0;

	touchwin(stdscr);
	refresh();

	initKeypad();

	delwin(board_win); // Clear data in arrays and reset terminal
	resetty();
	echo();
	endwin();
	free(info);
	free(board_arry);
	free(temp_arry);
	exit(EXIT_SUCCESS);
}

// Reset the terminal
void resetTerminal()
{
	delwin(board_win);
	resetty();
	echo();
	endwin();
	exit(EXIT_FAILURE);
}

// Print the game board
void printBoard(int physicalH, int physicalW)
{
	int i, j, k;
	bool horizontal = true;
	int tempIdx;
	unsigned char tempVal;
	chtype printChar;

	for (i = 0; i < physicalH; i++)
	{
		for (j = 0; j < physicalW; j++)
		{
			// Horizontal border
			if (horizontal)
			{
				if (i == 0)
				{
					// First border
					if (j == 0)
						printChar = ACS_ULCORNER;
					else if (j == physicalW - 1)
						printChar = ACS_URCORNER;
					else if (j % STEP_X == 0)
						printChar = ACS_TTEE;
					else
						printChar = ACS_HLINE;
				}
				else if (i == physicalH - 1)
				{
					// Last border
					if (j == 0)
						printChar = ACS_LLCORNER;
					else if (j == physicalW - 1)
						printChar = ACS_LRCORNER;
					else if (j % STEP_X == 0)
						printChar = ACS_BTEE;
					else
						printChar = ACS_HLINE;
				}
				else
				{
					// In the middle
					if (j == 0)
						printChar = ACS_LTEE;
					else if (j == physicalW - 1)
						printChar = ACS_RTEE;
					else if (j % STEP_X == 0)
						printChar = ACS_PLUS;
					else
						printChar = ACS_HLINE;
				}
			}
			else
			{
				if (j % (CW - 1) == 0)
					printChar = ACS_VLINE;
				else if (j % STEP_X == 2)
				{
					// This is a square
					tempIdx = (i / STEP_Y) * W + (j / STEP_X);
					tempVal = board_arry[tempIdx];
					if (tempVal & SHOW_MASK)
					{
						if ((tempVal & VALUE_MASK) == MINE)
						{
							if (COL_SUPP)
								wattron(board_win, A_BOLD);
							mvwaddch(board_win, i, j, MINE);
							if (COL_SUPP)
								wattroff(board_win, A_BOLD);
						}
						else if ((tempVal & VALUE_MASK) == ZERO)
						{
							mvwaddch(board_win, i, j, ' ');
						}
						else
						{
							if (COL_SUPP)
								wattrset(board_win, COLOR_PAIR((tempVal & VALUE_MASK) - ZERO));
							mvwaddch(board_win, i, j, (tempVal & VALUE_MASK));
							if (COL_SUPP)
								wattrset(board_win, COLOR_PAIR(DEFAULT_C));
						}
					}
					else if (tempVal & FLAG_MASK)
					{
						if (COL_SUPP)
							wattron(board_win, A_BOLD);
						mvwaddch(board_win, i, j, FLAG);
						if (COL_SUPP)
							wattroff(board_win, A_BOLD);
					}
					else
					{
						mvwaddch(board_win, i, j, ACS_CKBOARD);
					}
					continue;
				}
				else
					printChar = ' ';
			}
			mvwaddch(board_win, i, j, printChar);
		}

		horizontal = !horizontal;
	}
}

// Initialize the hame
void initGame()
{
	// Initialize to character 0
	memset(board_arry, ZERO, H * W);

	// Generate random mines
	int randIdx, randX, randY;
	srand(time(NULL));
	for (int i = 0; i < N; i++)
	{
		// Test for overlapping mines
		do
		{
			randIdx = rand() % (W * H);
		} while (board_arry[randIdx] == MINE);

		board_arry[randIdx] = MINE;
		randX = randIdx % W;
		randY = randIdx / W;

		// Update left 3 squares
		if (randX > 0)
		{
			if (randY > 0)
			{
				if (board_arry[randIdx - W - 1] != MINE)
					board_arry[randIdx - W - 1]++;
			}
			if (randY < H - 1)
			{
				if (board_arry[randIdx + W - 1] != MINE)
					board_arry[randIdx + W - 1]++;
			}
			if (board_arry[randIdx - 1] != MINE)
				board_arry[randIdx - 1]++;
		}

		// Update right 3 squares
		if (randX < W - 1)
		{
			if (randY > 0)
			{
				if (board_arry[randIdx - W + 1] != MINE)
					board_arry[randIdx - W + 1]++;
			}
			if (randY < H - 1)
			{
				if (board_arry[randIdx + W + 1] != MINE)
					board_arry[randIdx + W + 1]++;
			}
			if (board_arry[randIdx + 1] != MINE)
				board_arry[randIdx + 1]++;
		}

		// Update top and bottom squares
		if (randY > 0)
		{
			if (board_arry[randIdx - W] != MINE)
				board_arry[randIdx - W]++;
		}
		if (randY < H - 1)
		{
			if (board_arry[randIdx + W] != MINE)
				board_arry[randIdx + W]++;
		}
	}
}

// Initialize the game controls
void initKeypad()
{
	// Initailize keypad
	crmode();
	keypad(board_win, TRUE);
	noecho();
	int key;

	do
	{
		key = wgetch(board_win);
		if (GAME_END)
		{
			if (!GAME_END_PRESS--)
				break;
		}
		switch (key)
		{
		case KEY_LEFT:
		case 'a':
		case 'A':
			if (X - 1 >= 0)
				X--;
			break;
		case KEY_RIGHT:
		case 'd':
		case 'D':
			if (X + 1 < W)
				X++;
			break;
		case KEY_UP:
		case 'w':
		case 'W':
			if (Y - 1 >= 0)
				Y--;
			break;
		case KEY_DOWN:
		case 's':
		case 'S':
			if (Y + 1 < H)
				Y++;
			break;
		case 'f':
		case 'F':
			flagMine(board_win);
			break;
		case 'x':
		case 'X':
			wclear(stdscr);
			mainSetup();
			menuSetup();
		case ' ':
			testMine(board_win);
			break;
		default:
			break;
		}

		// Update cursor position
		wmove(board_win, Y * STEP_Y + 1, X * STEP_X + 2);
		touchwin(stdscr);
		refresh();

	} while (key != ERR && key != 'q' && key != 'Q');

	resetty();
	echo();
}

// Test for mines at a certain point on the game board
bool testMine()
{
	int idx = Y * W + X;
	if ((board_arry[idx] & SHOW_MASK) || (board_arry[idx] & FLAG_MASK))
	{
		// This value is already shown or is flaged
		return false;
	}
	else if ((board_arry[idx] & VALUE_MASK) == MINE)
	{
		// Oops, step over a mine
		gameOver(board_win);
		return true;
	}
	else
	{
		// If click number, just show itself
		if ((board_arry[idx] & VALUE_MASK) != ZERO)
		{
			if (COL_SUPP)
				wattrset(board_win, COLOR_PAIR((board_arry[idx] & VALUE_MASK) - ZERO));
			addChar(Y, X, board_arry[idx] & VALUE_MASK);
			board_arry[idx] |= SHOW_MASK;
			if (COL_SUPP)
				wattrset(board_win, COLOR_PAIR(DEFAULT_C));
			return false;
		}

		// Else find adjacent empty spaces and reveal them
		int counter = 0, i;
		int currX, currY, tempX, tempY;
		unsigned char *tempPtr;

		// Expand on empty spaces
		temp_arry[counter++] = Y;
		temp_arry[counter++] = X;
		while (counter > 0)
		{
			currX = temp_arry[--counter];
			currY = temp_arry[--counter];

			// Expand size
			if (counter + 10 >= tempSize)
			{
				tempSize *= 2;
				tempPtr = realloc(temp_arry, sizeof(unsigned char) * tempSize);
				if (tempPtr == NULL)
				{
					fprintf(stderr, "Can't allocate more space.\n");
					free(board_arry);
					free(temp_arry);
					free(info);
					resetTerminal();
					exit(1);
				}
				temp_arry = tempPtr;
				// fprintf(stderr, "Currsize: %d", tempSize);
			}

			// Show this square as empty
			addChar(currY, currX, ' ');
			board_arry[currY * W + currX] |= SHOW_MASK;

			// Update left 3 adjacent squares
			tempX = currX - 1;
			tempY = currY - 1;
			if (tempX >= 0)
			{
				for (i = 0; i < 3; i++, tempY++)
				{
					if (tempY < 0 || tempY >= H)
						continue;
					if (showSquare(tempY, tempX))
					{
						temp_arry[counter++] = tempY;
						temp_arry[counter++] = tempX;
					}
				}
			}

			// Update bottom square
			tempX = currX;
			tempY = currY + 1;
			if (tempY < H && showSquare(tempY, tempX))
			{
				temp_arry[counter++] = tempY;
				temp_arry[counter++] = tempX;
			}

			// Update right 3 adjacent squares
			tempX = currX + 1;
			tempY = currY - 1;
			if (tempX < W)
			{
				for (i = 0; i < 3; i++, tempY++)
				{
					if (tempY < 0 || tempY >= H)
						continue;
					if (showSquare(tempY, tempX))
					{
						temp_arry[counter++] = tempY;
						temp_arry[counter++] = tempX;
					}
				}
			}

			// Update top square
			tempX = currX;
			tempY = currY - 1;
			if (tempY >= 0 && showSquare(tempY, tempX))
			{
				temp_arry[counter++] = tempY;
				temp_arry[counter++] = tempX;
			}
		}

		// Reset color
		if (COL_SUPP)
			wattrset(board_win, COLOR_PAIR(DEFAULT_C));
	}

	return false;
}

// Show a square if valid and return whether its a empty square or not
bool showSquare(int currY, int currX)
{
	int tempIdx = currY * W + currX;
	unsigned char tempVal = board_arry[tempIdx];

	if (!(tempVal & SHOW_MASK) && !(tempVal & FLAG_MASK))
	{
		if ((tempVal & VALUE_MASK) != MINE)
		{
			if ((tempVal & VALUE_MASK) != ZERO)
			{
				if (COL_SUPP)
					wattrset(board_win, COLOR_PAIR((tempVal & VALUE_MASK) - ZERO));
				addChar(currY, currX, (tempVal & VALUE_MASK));
				board_arry[tempIdx] |= SHOW_MASK;
			}
			else
				return true;
		}
	}

	// Reset color
	if (COL_SUPP)
		wattrset(board_win, COLOR_PAIR(DEFAULT_C));

	return false;
}

// Add character depending on the type of square it is 
void addChar(int y, int x, chtype c)
{
	mvwaddch(board_win, y * STEP_Y + 1, x * STEP_X + 2, c);
}

// Print game related info
void printInfo()
{
	int rows, cols;
	getmaxyx(stdscr, rows, cols);

	move((W * STEP_X + 1) - 8, (cols - 40) / 2);
	clrtoeol();
	printw(info);
	touchwin(stdscr);
	refresh();
}

// Flag/Mark the locations of the mines
void flagMine()
{
	int tempIdx = Y * W + X;
	if (!(board_arry[tempIdx] & SHOW_MASK))
	{
		// Toggle flag
		if (board_arry[tempIdx] & FLAG_MASK)
		{
			if (COL_SUPP)
				wattroff(board_win, A_BOLD);
			addChar(Y, X, ACS_CKBOARD);
			board_arry[tempIdx] ^= FLAG_MASK;
			REMAIN_N++;

			// If unflag a mine
			if ((board_arry[tempIdx] & VALUE_MASK) == MINE)
				CORRECT_N--;
		}
		else
		{
			if (COL_SUPP)
				wattron(board_win, A_BOLD);
			addChar(Y, X, FLAG);
			board_arry[tempIdx] |= FLAG_MASK;
			REMAIN_N--;

			// If correctly flag a mine
			if ((board_arry[tempIdx] & VALUE_MASK) == MINE)
				CORRECT_N++;
		}
	}

	updateInfo();

	// Reset bold font
	if (COL_SUPP)
		wattroff(board_win, A_BOLD);

	// Test win condition
	if (CORRECT_N == N && REMAIN_N == 0)
	{
		sleep(0.5);
		gameWin();
	}
}

// Update game related info
void updateInfo()
{
	sprintf(info, infoFormat, REMAIN_N, CORRECT_N);
	printInfo();
}

// Reveal the locations of all the mines
void showAll()
{
	for (int i = 0; i < W * H; i++)
	{
		if ((board_arry[i] & FLAG_MASK) != FLAG_MASK)
		{
			if ((board_arry[i] & VALUE_MASK) != ZERO)
			{
				if (COL_SUPP)
				{
					if ((board_arry[i] & VALUE_MASK) != MINE)
						wattrset(board_win, COLOR_PAIR(board_arry[i] & VALUE_MASK - ZERO));
					else
						wattrset(board_win, (DEFAULT_C | A_BOLD));
				}
				addChar(i / W, i % W, board_arry[i] & VALUE_MASK);
			}
			else
			{
				if (COL_SUPP)
					wattrset(board_win, COLOR_PAIR(DEFAULT_C));
				addChar(i / W, i % W, ' ');
			}
		}
	}

	// Reset color
	if (COL_SUPP)
		wattrset(board_win, COLOR_PAIR(DEFAULT_C));
}

// When user wins game do this
void gameWin()
{
	showAll();
	GAME_END = true;
	GAME_END_PRESS = 1;

	// Inform the player they won
	sprintf(info, "Congrats! You survived the minefield POG");
	printInfo();

	sleep(3);
	GAME_END = false;
	GAME_END_PRESS = 0;
	main();
}

// When user loses do this
void gameOver()
{
	showAll();
	GAME_END = true;
	GAME_END_PRESS = 1;

	// Inform the player they lost
	sprintf(info, " You hit a mine! Better luck next time! ");
	printInfo();

	sleep(3);
	GAME_END = false;
	GAME_END_PRESS = 0;
	main();
}

// initialize color pairs
void initColorPairs()
{
	// If terminal supports color
	if (has_colors())
	{
		use_default_colors();
		if (start_color() != OK)
			return;
		COL_SUPP = true;
		init_pair(1, COLOR_GREEN, 0);
		init_pair(2, COLOR_YELLOW, 0);
		init_pair(3, COLOR_CYAN, 0);
		init_pair(4, COLOR_MAGENTA, 0);
		init_pair(5, COLOR_RED, 0);
		init_pair(6, COLOR_WHITE, 0);
	}
}
