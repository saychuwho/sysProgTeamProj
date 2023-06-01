/* 코드 사용법 
1. 컴파일 방법 
membrane.h와 membrane.c를 같이 넣은 후에 다음 명령어로 컴파일한다.
```bash
gcc -Wall -o keyMain membrane.c -lwiringPi
```

2. wiringPi 64bit용 설치
wiringPi 공식 사이트에서 제공하는 라이브러리는 32bit 버전이다. 과제1에서 우리는 64bit os를 깔았으므로 64bit용 wiringPi가 필요하다. 
따라서 다음 명령어를 통해 64bit용 wiringPi를 빌드해서 사용해야 한다.
```bash
git clone https://github.com/WiringPi/WiringPi.git
cd WiringPi
./build
```

3. 사용법
키패드의 1번부터 9번까지 abc/def/ghi/jkl/mno/pqr/stu/vwx/yz_ 로 입력 가능하다. 피쳐폰 숫자 키패드를 생각하면 됨.
A는 한글자 지우기, B는 Enter, C는 mode 변환이다.
문장을 다 입력한 후에 Enter를 누르면, 입력된 문장을 외부 텍스트 파일에 저장한 후 프로그램을 종료한다.
이는 응용하면 다양하게 활용가능하다. input_Question 함수가 입력받은 char* buffer에 문장을 저장하므로 buffer 속 데이터를 활용하면 됨.
C를 누르면 대문자, 소문자, 숫자 입력간 변환이 가능하다.

기타 오류 사항이나 troubleshooting이 필요하다면 저에게 말해주세요.
*/


#include <wiringPi.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "membrane.h"

// 가로 핀과 세로 핀이 어떻게 이루어져야 하는지를 나타냄
int rowPin[ROWS] = {6,13,19,26};
int colPin[COLS] = {12,16,20,21};

// 실제 키패드와 동일하다.
char keys[ROWS][COLS] = {
    {'1','2','3',127},
    {'4','5','6',10},
    {'7','8','9',46},
    {'*','0','#','D'}
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

// 어떤 프로그램을 짜던 간에, 키패드 입력을 하기 위해서는 이 함수를 불러와야 한다.
void init_keypad(){
    wiringPiSetupGpio();
    for(int i=0;i<ROWS;i++){
        pinMode(rowPin[i], OUTPUT);
    }

    for(int i=0;i<COLS;i++){
        pinMode(colPin[i], INPUT);
        pullUpDnControl(colPin[i], PUD_DOWN);    
    }
    printf("initiate keypad\n");
}

// 질문을 입력받는 함수
void input_Question(char* buffer){
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

// temp main : 이 부분은 키패드를 활용한 코드를 만들 때 대략 이런 flow를 따라야 한다는 예시이다.
// 실제 사용시에는 주석처리해 컴파일 에러가 안나도록 해야 한다.
int main(){
    setbuf(stdout, NULL);

    // init part
    init_keypad();

    
    // open file
    char filename[11] = "test00.txt";
    FILE *output;
    output = fopen(filename, "w");

    // malloc 활용
    char* buffer = (char *)malloc(sizeof(char) * 1024);
    
    // 문장을 받아오는 부분을 함수화하자.
    input_Question(buffer);
    
    // file에 buffer를 저장한다.
    buffer[strlen(buffer)] = '\n';
    fputs(buffer, output);
    
    
    // buffer를 저장했으니 free를 해주어야 한다.
    free(buffer);

    // file에 쓰는 것을 마무리한 이후에는 file을 닫아야 한다.
    fclose(output);

    return 0;
}