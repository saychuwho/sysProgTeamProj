#include <wiringPi.h>
#include <stdio.h>

#define ROWS 4
#define COLS 4

char pressedKey = '\0';
int rowPins[ROWS] = {29,28,27,26};
int colPins[COLS] = {25,24,23,22};

char keys[ROWS][COLS] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'},
};

void init_keypad(){
    for(int c=0; c < COLS; c++){
	pinMode(colPins[c], OUTPUT);
	digitalWrite(colPins[c], HIGH);
    }

    for(int r=0; r < ROWS; r++){	
	pinMode(rowPins[r], INPUT);
	digitalWrite(rowPins[r], PUD_UP);
    }
}

int findLowRow(){
    for(int r=0;r<ROWS;r++){
	if(digitalRead(rowPins[r]) == LOW)
	    return r;
    }
    return -1;
}

char get_key(){
    int rowIndex;

    for(int c=0;c<COLS;c++){
	digitalWrite(colPins[c], LOW);

	rowIndex = findLowRow();
	if(rowIndex > -1){
	    if(!pressedKey)
		pressedKey = keys[rowIndex][c];
	    return pressedKey;
	}

	digitalWrite(colPins[c], HIGH);
    }

    pressedKey = '\0';

    return pressedKey;
}

int main(void){
    wiringPiSetup();

    init_keypad();

    while(1){
	char x = get_key();

	if(x)
	    printf("pressed : %c\n", x);
	else
	    printf("no key pressed\n");
	delay(250);
    }

    return 0;
}

