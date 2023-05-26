#include <stdio.h>
#include <string.h>
#include <wiringPi.h>

#include "membrane.h"

// A는 backspace, B는 enter, C는 mode select
#define ENTER 98
#define BACKSPACE 97
#define MODESELECT 99

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
    
    int pressed_key[9] = {0,0,0,0,0,0,0,0,0};
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

    while(1){
		char ch = '\0';
		while(1){
			ch = '\0';
			ch = get_keys();
			delay(250);
			if(ch != '\0')
			break;
		}
		// 안전하게 하기 위한 code block
		if(ch != '\0'){
			int get_int = atoi(ch);
			if(get_int==ENTER && !(last_pressed_key==0)){
            // 가장 마지막으로 입력된 거를 출력해야 한다.
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
			else if(ch==ENTER && (last_pressed_key==0)){
				printf("\n");
				break;
			}
			else if(ch==BACKSPACE){
			buffer[strlen(buffer)-1] = '\0';
			}
			else if(ch==MODESELECT){
				mode++;
			}
			// 1:49 2:50 3:51 4:52 5:53 6:54 7:55 8:56 9:57 0:48 ' ':32
			else{
				last_pressed_key = ch-48;
				pressed_key[ch-48]++;
			}
			// 글자를 출력하는 부분
			if(ch==BACKSPACE){
				printf("\b\b ");
			}
			else if(ch==ENTER || ch==MODESELECT){
				continue;
			}
			else{
				char printing_char = translate_num(pressed_key, last_pressed_key, mode);
				if(printing_char==123)
					printing_char = 32;
				printf("\b%c", printing_char);
			}
		}
    }
    
}
