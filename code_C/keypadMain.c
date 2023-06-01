/*
-how to compile
    gcc -Wall -o keyMain ./src/keypadMain.c ./src/membrane.c -lwiringPi
*/

#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <unistd.h>

#include <term.h>
#include <termios.h>

#include "membrane_old.h"

// A는 backspace, B는 enter, C는 mode select, D는 글자입력
#define ENTER 10
#define BACKSPACE 127
#define MODESELECT 46
#define CHARIN 45

int getch(void)  
{  
  int ch;  
  struct termios buf;  
  struct termios save;  
  
   tcgetattr(0, &save);  
   buf = save;  
   buf.c_lflag &= ~(ICANON|ECHO);  
   buf.c_cc[VMIN] = 1;  
   buf.c_cc[VTIME] = 0;  
   tcsetattr(0, TCSAFLUSH, &buf);  
   ch = get_keys();  
   tcsetattr(0, TCSAFLUSH, &save);  
   return ch;  
}

// pushed_num에 있는 숫자들이 실제로 무슨 글자인지 해독하는 부분
char translate_num(int* pushed_num, int last_pushed, int mode){
    char temp = 0;
    if((mode%3) == 0){ // 소문자
        temp = 97 + (last_pushed-1)*3 + (*(pushed_num + last_pushed) % 3);
        if(temp==123){
            temp = 32;
        }
    }
    else if((mode%3) == 1){ // 숫자
	    temp = 48 + last_pushed;
    }
    else{ // 대문자
        temp = 65 + (last_pushed-1)*3 + (*(pushed_num + last_pushed) % 3);
        if(temp==91){
            temp = 32;
        }
    }
    return temp;
}


int main(void){
    wiringPiSetupGpio();
    init_keypad();
    
    int pressed_key[10] = {0,0,0,0,0,0,0,0,0,0};
    // 마지막으로 눌린 key를 판별
    int last_pressed_key = 0;
    // malloc을 안쓰는 선에서 이정도 문자열만 만들어둡시다.
    char buffer[1024] = "";
    // mode selector 0 : 소문자, 1 : 대문자, 2 : 숫자
    int mode = 0;

    char filename[11] = "test00.txt";

    // file로 내보내야 하니...
    FILE *output;
    output = fopen(filename, "w");
    
    // 문장을 하나 넣어서, 이때 입력이 제대로 되는지 확인해보자.
    //printf("[Input Question]: ");
    char ch = '\0';

    while(1){
	ch = getch();

	    // debug
	    //printf("DEBUG total : %c\n", ch);

	    int get_int = (int)ch;
	    if(get_int==CHARIN){
		// 가장 마지막으로 입력된 거를 출력해야 한다.
		// debug
		//printf("DEBUG : 1\n");

		char temp = translate_num(pressed_key, last_pressed_key, mode);
		if(temp == 123)
		    temp = 32;
		strncat(buffer, &temp, 1);
		printf(" ");
		// 수를 하나 입력한 후에는 꼭 초기화를 해주어야 한다.
		for(int i=0;i<10;i++)
		    pressed_key[i] = -1;
		last_pressed_key = 0;
	    }
	    else if(get_int==ENTER){
		// debug
		//printf("DEBUG : 2\n");
		printf("\n");
		break;
	    }
	    else if(get_int==BACKSPACE){
		// debug
		//printf("DEBUG : 3\n");
		buffer[strlen(buffer)-1] = '\0';
	    }
	    else if(get_int==MODESELECT){
		// debug
		//printf("DEBUG : 4\n");
		mode++;
	    }
	    // 1:49 2:50 3:51 4:52 5:53 6:54 7:55 8:56 9:57 0:48 ' ':32
	    else if(!(get_int==CHARIN)){
		// debug
		//printf("DEBUG : 5\n");
		last_pressed_key = get_int-48;
		pressed_key[get_int-48]++;
	    }
	    // 글자를 출력하는 부분
	    if(get_int==BACKSPACE){
		// debug
		//printf("DEBUG : 6\n");
		printf("\b\b ");
	    }
	    else if(get_int==ENTER || get_int==CHARIN || get_int==MODESELECT){
		// debug
		//printf("DEBUG : 7\n");
		continue;
	    }
	    else if(get_int != '\0'){
		// debug
		//printf("DEBUG : 8\n");
		char printing_char = translate_num(pressed_key, last_pressed_key, mode);
		if(printing_char==123)
		    printing_char = 32;
		printf("\b%c", printing_char);
	    }
    }
    // debug
    //printf("DEBUG : 9\n");
    buffer[strlen(buffer)] = '\n';
    fputs(buffer, output);
    fclose(output); 

    return 0;
}
