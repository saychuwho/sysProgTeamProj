#include <stdio.h>
#include <term.h>  
#include <termios.h>  
#include <unistd.h>  

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
- 마지막으로 누른 숫자키에 따라서 입력되어야 하는 알파벳 종류가 달라져야 한다. 이를 담는 곳 또한 필요하다. 
- 그리고 엔터키가 눌리면 최종적으로 그 문자가 화면에 나오도록 만들어야 한다.
- 이 과정을 while로 돌리면 되지 않을까.
*/


int main(){
    int pushed_num[10];
    for(int i=0;i<10;i++)
        pushed_num[i] = 0;
    int last_pushed = 0;
    
    while(ture){

    }
    
    return 0;
}