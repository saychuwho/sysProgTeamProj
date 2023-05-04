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
  
int main(void)  
{  
    printf("%c\n", 97);
    int ch;  
  
    for(; !(ch=='\n');){  
  
        ch = getch();  
        printf("%d \n", ch);  
    }  
  
    return 0;  
}