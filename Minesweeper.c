#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <curses.h>
#include <sys/ioctl.h>


int terminalSize(void){
	
	struct winsize ws;
	ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
	
	return ws.ws_row,ws.ws_col;
}


int main() {
	
	initscr();
	refresh();
	cbreak();
	
	int color = 1;
	
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(4, COLOR_CYAN, COLOR_BLACK);
	init_pair(5, COLOR_BLACK, COLOR_YELLOW);
	init_pair(6, COLOR_WHITE, COLOR_BLACK);
	attron(COLOR_PAIR(color));
	
	
	int width,height = terminalSize();
	
	int start_x = 0,start_y = 0;
	
	char msg[]="This is My Box";		
	int row,col;
	
	WINDOW * win = newwin(height,width,start_y,start_x);
	refresh();
	
	FILE *p = fopen ("logo.txt" , "r");;
	char str[200];
	int i = 1;

	while(fgets (str, 200, p) != NULL){
		mvprintw(i++, COLS / 2 - 50, "%s", str);
		wrefresh(win);
	}
	attron(A_STANDOUT);
	wborder(win, ' ',' ',' ',' ',' ',' ',' ',' ');
	attroff(A_STANDOUT);
	wrefresh(win);
    fclose (p);
	
	attron(COLOR_PAIR(4));
	wborder(win, '*','*','*','*','*','*','*','*');
	attroff(COLOR_PAIR(4));
	wrefresh(win);

	getch();
	
	endwin();
	
	return 0;
}