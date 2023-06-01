#include <wiringPi.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "membrane.h"

int rowPin[ROWS] = {6,13,19,26};
int colPin[COLS] = {12,16,20,21};

char keys[ROWS][COLS] = {
    {'1','2','3',127},
    {'4','5','6',10},
    {'7','8','9',46},
    {'*','0','#',45}
};

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

void init_keypad(){
    printf("initiate keypad\n");
    for(int i=0;i<ROWS;i++){
        pinMode(rowPin[i], OUTPUT);
    }

    for(int i=0;i<COLS;i++){
        pinMode(colPin[i], INPUT);
        pullUpDnControl(colPin[i], PUD_DOWN);    
    }
}

void input_Q(char* buffer){
    // code from TempKeypadCon.c
    int pushed_num[10];
    for(int i=0;i<10;i++)
        pushed_num[i] = -1;
    int last_pushed = 0;
    int mode = 0;
    
    // using char
    char ch=0;
    unsigned int isEnd = 0;

    printf("Input Question : ");
    while(isEnd == 0){
        for(int row=0;row<ROWS;row++){
            digitalWrite(rowPin[row], HIGH);
            for(int col=0;col<COLS;col++){
                if(digitalRead(colPin[col]) == HIGH){
                    ch = keys[row][col];
                    //printf("printed key : %c", ch);
                    
                    if(ch != 0){
                        // code from TempKeypadCon.c
                        if(ch==ENTER && !(last_pushed==0)){
                            // 가장 마지막으로 입력된 거를 출력해야 한다.
                            char temp = translate_num(pushed_num, last_pushed, mode);
                            if(temp == 123)
                                temp = 32;
                            strncat(buffer, &temp, 1);
                            printf(" ");
                            // 수를 하나 입력한 후에는 꼭 초기화를 해주어야 한다.
                            for(int i=0;i<10;i++)
                                pushed_num[i] = -1;
                            last_pushed = 0;
                        }
                        else if(ch==ENTER && (last_pushed==0)){
                            printf("\n");
                            isEnd = 1;
                        }
                        else if(ch==BACKSPACE){
                        buffer[strlen(buffer)-1] = '\0';
                        }
                        else if(ch==MODESELECT){
                            mode++;
                        }
                        // 1:49 2:50 3:51 4:52 5:53 6:54 7:55 8:56 9:57 0:48 ' ':32
                        else{
                            last_pushed = ch-48;
                            pushed_num[ch-48]++;
                        }
                        // 글자를 출력하는 부분
                        if(ch==BACKSPACE){
                            printf("\b\b ");
                        }
                        else if(!(ch==ENTER) && !(ch==MODESELECT)){
                            char printing_char = translate_num(pushed_num, last_pushed, mode);
                            if(printing_char==123)
                                printing_char = 32;
                            printf("\b%c", printing_char);
                        }
                    }
                }
            }
            digitalWrite(rowPin[row], LOW);
        }
        delay(200);
    }
}

// temp main
int main(){
    setbuf(stdout, NULL);

    // init part
    wiringPiSetupGpio();
    init_keypad();

    
    // open file
    char filename[11] = "test00.txt";
    FILE *output;
    output = fopen(filename, "w");
    
    // malloc을 사용할 줄 알면 가변 배열을 이용할거 같은데, 일단은 크기를 1024로 설정한 buffer를 만든다
    // char buffer[1024] = "";

    // malloc 활용
    char* buffer = (char *)malloc(sizeof(char) * 1024);
    
    // 문장을 받아오는 부분을 함수화하자.
    input_Q(buffer);
    
    // file에 buffer를 저장한다.
    buffer[strlen(buffer)] = '\n';
    fputs(buffer, output);
    
    
    // buffer를 저장했으니 free를 해주어야 한다.
    free(buffer);

    // file에 쓰는 것을 마무리한 이후에는 file을 닫아야 한다.
    fclose(output);

    return 0;
}