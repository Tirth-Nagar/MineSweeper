#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <curses.h>

int menuSetup(void){
	int start_y=0,start_x=0;
	
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(4, COLOR_CYAN, COLOR_BLACK);
	init_pair(5, COLOR_BLACK, COLOR_YELLOW);
	init_pair(6, COLOR_WHITE, COLOR_BLACK);
	
	int width = 0,height = 0;
	getmaxyx(stdscr,height,width);
	
	
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

    fclose (p);
	
	start_y = 8, start_x = 0;
	
	WINDOW * opt = newwin(height-start_y,width,start_y,start_x);
	refresh();
	
	keypad(opt,true);
	
	box(opt,0,0);
	
	wrefresh(opt);
	
	char *options1[4] = {"New game", "Settings", "About", "Quit"};
	
	int choice, highlight = 0;
	
	while(1){
		for(int i =0; i<4;i++){
			if(i==highlight){
				wattron(opt,A_REVERSE);
			}
			mvwprintw(opt,i+1, 1, options1[i]);
			wattroff(opt,A_REVERSE);
			
		}
		choice = wgetch(opt);
		
		switch(choice){
			case KEY_UP:
				highlight--;
				break;
			case KEY_DOWN:
				highlight++;
				break;
			default:
				break;
		}
		
		if(choice==10){
			break;
			
			
		}
		
		
		
		
	}
	
	
	
	return 0;
}



int main() {
	
	initscr();
	noecho();
	cbreak();
	
	menuSetup();
	

	getch();
	
	endwin();
	
	return 0;
}
