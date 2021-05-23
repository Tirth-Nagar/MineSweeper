#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <curses.h>

int mainSetup(void){
	
	int start_y=0,start_x=0;
	
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(4, COLOR_CYAN, COLOR_BLACK);
	init_pair(5, COLOR_BLACK, COLOR_YELLOW);
	init_pair(6, COLOR_WHITE, COLOR_BLACK);
	init_pair(7, COLOR_RED, COLOR_WHITE);
	
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

	return 0;
}

int menuSetup(void){
	
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(4, COLOR_CYAN, COLOR_BLACK);
	init_pair(5, COLOR_BLACK, COLOR_YELLOW);
	init_pair(6, COLOR_WHITE, COLOR_BLACK);
	init_pair(7, COLOR_RED, COLOR_WHITE);
	
	int width = 0,height = 0;
	getmaxyx(stdscr,height,width);
	
	int start_y = 8, start_x = 0;
	
	WINDOW * opt = newwin(height-start_y,width,start_y,start_x);
	refresh();
	
	keypad(opt,true);
	
	getmaxyx(opt,width,height);	
	
	box(opt,0,0);
	
	wrefresh(opt);
	
	char *options1[4] = {"New game", "Settings", "About", "Quit"};
	int charNum[4] = {8,8,5,4};
	int choice, highlight = 0;
	
	while(1){
		for(int i=0; i<4;i++){
			if(i==highlight){
				wattron(opt,A_REVERSE);
				wattron(opt,COLOR_PAIR(1));
			}
			mvwprintw(opt,width/2+i, (height- charNum[i])/2, "%s", options1[i]);
			wrefresh(opt);
			wattroff(opt,COLOR_PAIR(1));
			wattroff(opt,A_REVERSE);
		}
		choice = wgetch(opt);
		
		switch(choice){
			case KEY_UP:
				highlight--;
				if(highlight==-1){
					highlight = 3;
				}
				break;
			case KEY_DOWN:
				highlight++;
				if(highlight==4){
					highlight = 0;
				}
				break;
			default:
				break;
		}
		
		if(choice==10){
			break;
		}
	}
	
	wclear(opt);
	refresh();
	
	box(opt,0,0);
	wrefresh(opt);
	
	if(highlight==0){
		NULL; //New Game
	}
	
	else if(highlight==1){
		NULL; //Settings
	}
	
	else if(highlight==2){
		wattron(opt,A_UNDERLINE);
		mvwprintw(opt,width/2, (height-38)/2,"Hello welcome to the minesweeper clone");
		mvwprintw(opt,width/2+1, (height-36)/2,"made by Tirth Nagar using the Curses");
		mvwprintw(opt,width/2+2, (height-40)/2,"Library the rules are the same as normal");
		mvwprintw(opt,width/2+3, (height-27)/2,"Minesweeper so Go Have Fun!"); //About
		wattroff(opt,A_UNDERLINE);
		
		wattron(opt,A_STANDOUT);
		mvwprintw(opt,width/2+5,(height-22)/2," Press 'X' To Go Back ");
		wattroff(opt,A_STANDOUT);
		wrefresh(opt);
		
		char ch;
		while(ch!='X'||ch!='x'){ 
			ch = getch();
			
			if(ch == 'x' || ch == 'X'){
				wclear(stdscr);
				mainSetup();
				menuSetup();
			}
		}
	}
	
	else if(highlight==3){
		echo();
		refresh();
		endwin();
		erase();
		exit(0); //Quit
	}
	
	return 0;
}

int main() {
	
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
