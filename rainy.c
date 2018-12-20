/* 
 * project	: 산성비 게임
 * description  : fork, pthread.h, curese.h 함수를 이용해 구현하였습니다.
 */

#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <curses.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include <ctype.h>

//
//display할 문자의 내용과 가로, 세로 위치를 저장할 구조체를 선언
//
typedef struct word* wordptr;
typedef struct word{
        char str[30];
        int row;
        int col;
}word;

void create();
void firstscreen();
int menu();
void play();
void movestr();
wordptr random_input();
void *answ_input();
void cmplist(char []);
int readranking(int *, char[][100]);
void sort(int *, char[][100], int);
void printrank(int *, char[][100], int);
void ranking();

pthread_mutex_t counter_lock = PTHREAD_MUTEX_INITIALIZER;
wordptr Word_Arr;
int done=0;			//game over인지 아닌지 판별
wordptr list[300];		//단어 struct를 저장할 pointer array
int listpoint=0;		//list[]의 포인터로 사용
char noansw[30]="nthansw";	//유저가 단어를 맞출 경우 이 문자를 대신 넣고, 게임에서 출력하지 않음
int score=0;			//유저의 점수
char scorebuf[3]={0};		//addstr에 넣기 위한 buffer

int main(){
        int choice;
	int loop=1;
	int pid;

        firstscreen();
	while(loop){
        	choice = menu();        	//int 형으로 return 하니 이를 통해 이후 조작 가능

       		if( (pid=fork())==-1){		//이후 함수 호출한 후 다시 메뉴로 돌아오도록 fork
			perror("fork");		
			exit(1);
		}
		else if(pid==0){
			switch(choice){
        		        case 1:
					play();		//게임 시작
					exit(0);
					break;
        		        case 2:
	      		                ranking();	//랭킹 메뉴
					exit(0);
        		                break;
        		        default:
					exit(0);	//종료
        		                break;
			}
        	}
		else{
			if(choice==3)
				loop=0;			//종료
			wait(NULL);
		}
			
	}
        return 0;
}
/*
 * 처음 화면, 게임 이름과 조원 이름을 출력하고
 * 아무 키나 입력 받은 후 종료
 *
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
/*
 * 게임 실행 함수. 처음에 ID를 무작위로 생성하여 출력해줌
 * 이후 thread를 생성하여 문자를 하나씩 추가해가며 시간마다 내려오도록 하는 함수(movestr)와
 * 유저에게 값을 입력받고 정답을 비교하는 함수(answ_input)를 동시에 실행시킴
 * 단어가 맨 아래로 내려와 게임이 끝나면 gameover창과 함께 점수를 보여주고
 * 아무 값이나 입력받은 후에 종료시킴
 */
void play(){
	FILE *fpout;				//ranking.txt에 출력할 스트림
        int delay=1000000, cnt=1;		//delay는 단어가 내려오는 초기속도, cnt는 속도변화
	char ch[4]={0}, ch2[4]={0};			
	pthread_t t1;				//thread
	srand(time(NULL));			
        initscr();
	create();				//이후 문자를 파일에서 읽어오기 위한 함수
        noecho();
        clear();
//
//	rand()를 이용해 사용자 이름을 playerxxx로 정하고 출력.
//	2초 후 종료되고 게임 시작
//
	cnt=rand()%1000;
	ch[0]=cnt/100+48;		//숫자를 문자열에 넣어 addstr에 쓸 수 있게 함	
	ch[1]=(cnt%100)/10+48;
	ch[2]=cnt%10+48;
	ch[3]='\0';
	cnt=1;
	mvaddstr(LINES/2-1, COLS/2-10, "Your ID : player");
	addstr(ch);
	refresh();
	sleep(2);
//
//	thread를 이용해 answ_input과 movestr을 동시에 실행시킴
//	단어 하나가 맨 밑에 내려오면 done=1이 되며 종료
//
	clear();
	pthread_create(&t1, NULL, answ_input, NULL);
        while(done==0){
		movestr();
		if(cnt>47)
			usleep(200000);				//최대속도는 0.2s
		else{
			usleep(delay-(cnt/6)*(delay/10));	//단어 6개가 내려온 이후 속도 변화
			cnt++;
		}
	}
//
//	game over 이후 ID와 점수를 출력해주고
//	ranking.txt에 append모드로 출력
//
	pthread_mutex_lock(&counter_lock);
	clear();
	mvaddstr(LINES/2-2, COLS/2-5, "Game over");
	mvaddstr(LINES/2-1, COLS/2-6, "ID : player");
	addstr(ch);
	mvaddstr(LINES/2, COLS/2-7,   "SCORE : ");
	sprintf(ch2, "%d", score);
	addstr(ch2);
	standout();
	mvaddstr(LINES-1, 0, "Press any key");
	standend();
	getch();
	pthread_mutex_unlock(&counter_lock);
        endwin();
	
	fpout=fopen("ranking.txt", "a");
	fprintf(fpout, "%d player%s\n", score, ch);
	score=0;
}
/*
 * 새로운 단어를 하나 읽어오고, 기존 단어들을 모두 한줄씩 내려 출력하는 함수
 */
void movestr()
{
	int i;
        list[listpoint++]=random_input();		//새로운 단어를 추가
        int yn;
        for (i=0;i<listpoint;i++){
		yn=strcmp(list[i]->str, noansw);	//이미 맞춘 단어는 출력하지 않음
                if(list[i]->row==LINES-4&&yn!=0)	//맨 밑에 내려온 단어는 출력하지 않음
                        done=1;
		else{
			pthread_mutex_lock(&counter_lock);	//출력은 mutex_lock해줘야 함

			if(yn!=0 || list[i]->row<LINES-4)	//이전에 출력한 단어는 지워줌
				mvaddstr(list[i]->row-1, list[i]->col, "                      ");

			if(yn!=0)				//한줄 내려 출력
                		mvaddstr(list[i]->row, list[i]->col, list[i]->str);
				move(LINES-1, 0);
                	refresh();
			pthread_mutex_unlock(&counter_lock);
                	list[i]->row+=1;
		}
	}
}
/*
 * 단어를 한 글자씩 입력받아 그대로 출력해주고,
 * 엔터를 입력받은 경우 정답인지 판별해 정답인 경우 list의 해당 단어를 지움
 * 백스페이스를 입력받으면 한글자를 지워줌
 */

void *answ_input(){
	int i, answptr=0;
	char answ[30]={0}, c;	//단어를 입력받을 배열
//
//layout작성	
//
	pthread_mutex_lock(&counter_lock);
        for(i=0; i<COLS-1; i++)
                mvaddstr(LINES-3, i, "-");
	refresh();
	pthread_mutex_unlock(&counter_lock);

	while(done==0){
//
//layout 작성
//
		pthread_mutex_lock(&counter_lock);
		mvaddstr(LINES-2, 0, "ANSWER :");
                mvaddstr(LINES-2, COLS-16,"SCORE : ");
		sprintf(scorebuf, "%d", score);
		addstr(scorebuf);
                refresh();

		pthread_mutex_unlock(&counter_lock);
		c=getch();				//한 글자 입력
		pthread_mutex_lock(&counter_lock);
//
//엔터인 경우 단어를 비교하고(cmplist)
//answ[], answptr초기화해줌
//
		if(c=='\n'){
			answ[answptr]='\0';
			cmplist(answ);

			for(i=0;i<50;i++)
				answ[i]='\0';		//answ[]초기화
			answptr=0;			//answptr 초기화

			mvaddstr(LINES-2, 0, "ANSWER :                 ");
			mvaddstr(LINES-2, COLS-16,"SCORE : ");
			sprintf(scorebuf, "%d", score);
                	addstr(scorebuf);
			refresh();
		}
//
//백스페이스일 경우 한글자를 지움
//
		else if(c==127&&answptr>0){
			answ[--answptr]='\0';		//answ 값 지움
			mvaddstr(LINES-2, 0, "ANSWER : ");
			addstr(answ);	

			addstr(" ");			//display된 글자 지움

                        mvaddstr(LINES-2, COLS-16,"SCORE : ");
			sprintf(scorebuf, "%d", score);
                	addstr(scorebuf);
			refresh();
		}
//
//입력받은 글자를 배열에 넣음
//
		else{
			answ[answptr++]=c;	
			mvaddstr(LINES-2, 0, "ANSWER : ");
                        addstr(answ);
                        mvaddstr(LINES-2, COLS-16,"SCORE : ");
			sprintf(scorebuf, "%d", score);
                	addstr(scorebuf);
                        refresh();
		}	
		move(LINES-1, 0);
		refresh();
		pthread_mutex_unlock(&counter_lock);
	}
}
/*
 * answ_input에서 넘긴 단어가 list[]에 있는지 확인해서 있는 경우
 * list의 해당단어를 삭제(noansw로 내용 변환)
 */
void cmplist(char answ[]){
	int i;
	for(i=0;i<listpoint;i++){
		if(strcmp(answ,list[i]->str)==0){
			strcpy(list[i]->str, noansw);
			score++;
			break;
		}
	}
}
/*
 * WordData.txt 내용을 전부 읽어오고, 초기 위치를 임의로 정해 저장
 */
void create(){
        FILE *fp = fopen("WordData.txt","r");
        int size = 1000;
        wordptr temp_Arr;

        Word_Arr = (word*)malloc(sizeof(word)*size);

        temp_Arr = Word_Arr;
        int random;
        for(int i=0; i<size; i++)
        {
                fscanf(fp, "%s", &temp_Arr->str);
                random = rand()%(COLS-20); //떨어지는 위치
                temp_Arr->col = random;
                temp_Arr->row = 0;
                temp_Arr++;
        }
}
/*
 * 임의로 한 단어를 골라 return해줌 
 */
wordptr random_input(){
        wordptr temp_Arr;
        int random;
        int size = 1000;

        random = rand()%(size-1) + 1; //맨 앞 a는 빼고 보냄

        temp_Arr = Word_Arr;
        for(int i=0; i<random; i++)
                temp_Arr++;
        return temp_Arr;
}
/*
 * ranking.txt를 읽어와 점수에 따라 사용자를 sort한 후 
 * 출력해줌
 */
void ranking() {
        int rank[32];
        int people;
        char name[30][100];
        int i;

        people = readranking(rank, name);

        sort(rank, name, people);

        printrank(rank, name, people);
}
/*
 * ranking.txt를 읽어 name에 저장하고, 기록된 사용자 수를 return
 */
int readranking(int *rank, char name[][100]) {
        FILE* fp;
        int cnt = 0;

        close(0);
        if((fp = fopen("ranking.txt", "r")) == NULL) {
                perror("ranking.txt");
                exit(1);
        }

        while(!feof(fp)) {
                fscanf(fp, "%d %s\n", rank + cnt, name[cnt]);
                cnt++;
        }

        return cnt;
}
/*
 * name을 sort
 */
void sort(int *rank, char name[][100], int cnt) {
        int i, j, temp;
        char ntemp[100];

        for(i = 0; i < cnt - 1; i++) {
                for(j = i + 1; j < cnt; j++) {
                        if(rank[i] < rank[j]) {
                                temp = rank[i];
                                rank[i] = rank[j];
                                rank[j] = temp;
                                strcpy(ntemp, name[i]);
                                strcpy(name[i], name[j]);
                                strcpy(name[j], ntemp);
                        }
                }
        }
}
/*
 * name 내용을 출력해주고, 5초 뒤에 종료
 */
void printrank(int *rank, char name[][100], int cnt) {
        signal(SIGINT, SIG_IGN);
        signal(SIGQUIT, SIG_IGN);

        int i;
        char num[2], temp[32][144];
        char m = 0;

        initscr();
        noecho();
        clear();

        move(LINES/2-4, COLS/2-12);
        addstr("======ranking======");
//
// name 출력
//
        for(i = 0; i < cnt; i++) {
                sprintf(temp[i], "%d", rank[i]);
                move(LINES/2-2+i, COLS/2-12);
                sprintf(num, "%d", i + 1);
                addstr(num);
                addstr(". ");
                addstr(name[i]);
                addstr(": ");
                addstr(temp[i]);
        }
//
// 5초 뒤 종료
//
        move(LINES-1, 0);
        standout();
        addstr("This page will be shut down in 5 seconds");
        standend();
        refresh();
        sleep(5);
        endwin();
}

