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
	
	int start_y=0,start_x=0;
	
	start_color();
	init_pair(1, 2, 0);
	init_pair(2, 1, 0);
	init_pair(3, 5, 0);
	init_pair(4, 6, 0);
	init_pair(5, 0, 3);
	init_pair(6, 7, 0);
	
	int width,height = terminalSize();
	
	
	WINDOW * win = newwin(height,width,start_y,start_x);
	refresh();
	
	FILE *p = fopen ("logo.txt" , "r");;
	char str[200];
	int i = 1;

	while(fgets (str, 200, p) != NULL){
		attron(COLOR_PAIR(1));
		mvprintw(i++, COLS / 2 - 50, "%s", str);
		attroff(COLOR_PAIR(1));
		wrefresh(win);
	}
	//attron(COLOR_PAIR(7));
	//wborder(win, ' ',' ',' ',' ',' ',' ',' ',' ');
	//attroff(COLOR_PAIR(7));
	//wrefresh(win);
    fclose (p);
	
	start_y = 8, start_x = 0;
	
	WINDOW * opt = newwin(height-start_y,width,start_y,start_x);
	refresh();
	
	attron(A_BOLD);
	box(opt,0,0); //Doesn't show bottom border
	//wborder(opt, '*','*','*','*','*','*','*','*'); //Doesn't show bottom border
	attroff(A_BOLD);
	wrefresh(opt);

	getch();
	getch();
	
	endwin();
	
	return 0;
}
