/* author       : 김현유
 * date         : 2018-11-22 19:14
 * description  : 처음 화면, 메뉴 선택 화면에 대한 코드입니다
 */

#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <curses.h>

/* 
 * 처음 화면. 제목과 설명, 팀원들이 표시된다.
 */
void firstscreen(){

        initscr();
                clear();
                move(LINES/2-2, COLS/2-12);
                addstr("====rainy typing game====");
		move(LINES/2-1, COLS/2-7);
                addstr("system programing");
		move(LINES/2, COLS/2-5);
                addstr("team project");
		move(LINES-3 , 0);	
                addstr("member : Kim hyeonyu, Song yeonwuk, Kim kyeonglae, Hong heokjin");
		move(LINES-1, 0);
		standout();
		addstr("press any key to continue");
		standend();
        refresh();
        getch();
        endwin();
}

/* 메뉴 선정 화면 
 * 종료 시그널을 무시하도록 handler 변경
 * echo를 끄고 사용자에게 1,2,3중 하나를 선택하도록 한다.
 * 해당 숫자가 입력될 때까지 입력 받는다.
 */
int menu(){
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	
	char select=0;
		
	initscr();
	noecho();
	clear();
		move(LINES/2-4, COLS/2-12);
		addstr("========================");
		move(LINES/2-3, COLS/2-12);
                addstr("          menu          ");
		move(LINES/2-2, COLS/2-12);
                addstr("========================");
                move(LINES/2-1, COLS/2-12);
                addstr("      1.Start game      ");
		move(LINES/2, COLS/2-12);
                addstr("      2.Ranking         ");
		move(LINES/2+1, COLS/2-12);
                addstr("      3.End             ");
	refresh();
		move(LINES-1, 0);
		standout();
		addstr("enter the number of a menu");
		standend();
	refresh(); 
	select = getch();
	
	while(1){
		if(select==49||select==50||select==51)
			break;
		move(LINES-1, 0);
		standout();
		addstr("Wrong input. Enter again  ");
		standend();
		refresh();
		select = getch();
	}
	clear();
	endwin();
	
	return select-48;
                
}
int main(){
	int choice;

	firstscreen();
	choice = menu();	//int 형으로 return 하니 이를 통해 이후 조작 가능
	
	return 0;
}
