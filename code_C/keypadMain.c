#include <stdio.h>
#include <string.h>
#include <wiringPi.h>

#include "membrane.h"

// A는 backspace, B는 enter, C는 mode select

// pushed_num에 있는 숫자들이 실제로 무슨 글자인지 해독하는 부분
char translate_num(int* pushed_num, int last_pushed, int mode){

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
    int mode_selector = 0;

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
	    
	}
    }
    
}
