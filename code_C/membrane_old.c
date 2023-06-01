#include <wiringPi.h>
#include <stdio.h>
#include <unistd.h>

#include "membrane_old.h"


char keys[ROWS][COLS] = {
    {'1','2','3',127},
    {'4','5','6',10},
    {'7','8','9',46},
    {'*','0','#',45}
};


void init_keypad(){
    pinMode(L1, OUTPUT);
    pinMode(L2, OUTPUT);
    pinMode(L3, OUTPUT);
    pinMode(L4, OUTPUT);

    pinMode(C1, INPUT);
    pullUpDnControl(C1, PUD_DOWN);
    pinMode(C2, INPUT);
    pullUpDnControl(C2, PUD_DOWN);
    pinMode(C3, INPUT);
    pullUpDnControl(C3, PUD_DOWN);
    pinMode(C4, INPUT);
    pullUpDnControl(C4, PUD_DOWN);
}

// int를 return하도록 변형
// 그리고 tmp 포인터를 받아서 return하지 말고, pointer 값을 바꾸도록 만들자.
void readLine(int line, char* characters, char* tmp){
    digitalWrite(line, HIGH);
    if(digitalRead(C1) == HIGH){
	*tmp = characters[0];
	digitalWrite(line, LOW);
    }
    if(digitalRead(C2) == HIGH){
	*tmp = characters[1];
	digitalWrite(line, LOW);
    }
    if(digitalRead(C3) == HIGH){
	*tmp = characters[2];
	digitalWrite(line, LOW);
    }
    if(digitalRead(C4) == HIGH){
	*tmp = characters[3];
	digitalWrite(line, LOW);
    }
    digitalWrite(line, LOW);
}

char get_keys(){
    char tmp= '\0';
    readLine(L1, keys[0], &tmp);
    readLine(L2, keys[1], &tmp);
    readLine(L3, keys[2], &tmp);
    readLine(L4, keys[3], &tmp);
    return tmp;
}

