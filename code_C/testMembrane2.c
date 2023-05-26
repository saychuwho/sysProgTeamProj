#include <wiringPi.h>
#include <stdio.h>

#define ROWS 4
#define COLS 4

#define L1 6
#define L2 13
#define L3 19
#define L4 26

#define C1 12
#define C2 16
#define C3 20
#define C4 21

char keys[ROWS][COLS] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
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

void readLine(int line, char* characters){
    digitalWrite(line, HIGH);
    if(digitalRead(C1) == HIGH)
	printf("pressed: %c\n", characters[0]);
    if(digitalRead(C2) == HIGH)
	printf("pressed: %c\n", characters[1]);
    if(digitalRead(C3) == HIGH)
	printf("pressed: %c\n", characters[2]);
    if(digitalRead(C4) == HIGH)
	printf("pressed: %c\n", characters[3]);
    digitalWrite(line, LOW);
}

int main(void){
    wiringPiSetupGpio();
    init_keypad();
    while(1){
	readLine(L1, keys[0]);
	readLine(L2, keys[1]);
	readLine(L3, keys[2]);
	readLine(L4, keys[3]);
	delay(250);
    }
    return 0;    
}
