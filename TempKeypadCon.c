#include <stdio.h>
#include <term.h>  
#include <termios.h>  
#include <unistd.h>  
#include <string.h>

// 사용되는 상수들을 define하는 곳
#define ENTER 10
#define BACKSPACE 127
#define MODESELECT 46

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
   ch = getchar();  
   tcsetattr(0, TCSAFLUSH, &save);  
   return ch;  
}

/*how to make this
- 어떤 key가 입력되면, 그 key가 눌릴 때 마다 증가하는 수가 늘어난다. abc / def / ... 이런식으로 누를 때 마다 입력되는 알파벳이 달라지기 때문에 이를 구현한다.
- 마지막으로 누른 숫자키에 따라서 입력되어야 하는 알파벳 종류가 달라져야 한다.
- 그리고 엔터키가 눌리면 최종적으로 그 문자가 화면에 나오도록 만들어야 한다.
- 이 과정을 while로 돌리면 되지 않을까.
- backspace(127)가 들어오면 글자를 지운다.
- mode 변환이 필요하다. mode 변환은 0을 눌렀을때로 설정하고, mode 변환이 되었을때는 숫자를 입력하도록 바꿔야 한다.
- 이를 응용하면 대문자, 소문자도 입력받을 수 있도록 만들 수 있다.
- 1:abc 2:def 3:ghi 4:jkl 5:mno 6:pqr 7:stu 8:vwx 9:yz_
- 아무 숫자도 입력하지 않고 enter를 입력하면 버퍼에 있는 문자열을 파일에 기록하고 프로그램을 종료한다.
- 숫자 입력이 완료된 이후 enter가 들어오면 해당하는 글자가 버퍼에 들어간다
- 아무 숫자가 입력이 안된채로 엔터가 눌리면 그대로 버퍼가 파일에 출력된다.
- 파일에는 문장 하나씩 집어넣어야 한다. 파일 이름의 맨 끝에 char 2개를 조절해가면서 수를 표현하면 될 듯. 일단은 1자리부터 test.
*/

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

int main(){
    int pushed_num[10];
    for(int i=0;i<10;i++)
        pushed_num[i] = -1;
    int last_pushed = 0;
    int mode = 0;
    char filename[11] = "test00.txt";
    // malloc을 사용할 줄 알면 가변 배열을 이용할거 같은데, 일단은 크기를 1024로 설정한 buffer를 만든다
    char buffer[1024] = "";
    
    // 일단은 1줄을 받는 코드부터 구현
    int ch = 0;
 
    FILE *output;
    output = fopen(filename, "w");

    while(1){
        ch = getch();
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
            last_pushed = ch-48;
            pushed_num[ch-48]++;
        }
        // 글자를 출력하는 부분
	if(ch==BACKSPACE){
	    printf("\b\b ");
	}
	else if(ch==ENTER || ch==MODESELECT){
	    continue;
	}
	else{
            char printing_char = translate_num(pushed_num, last_pushed, mode);
            if(printing_char==123)
                printing_char = 32;
            printf("\b%c", printing_char);
        }
    }
    buffer[strlen(buffer)] = '\n';
    fputs(buffer, output);
    fclose(output);
    
    return 0;
}
