#ifndef MEMBRANE
#define MEMBRANE

// A는 backspace, B는 enter, C는 mode select, D는 글자입력
#define ENTER 10
#define BACKSPACE 127
#define MODESELECT 46

#define ROWS 4
#define COLS 4

char translate_num(int* pushed_num, int last_pushed, int mode);

void init_keypad();

void input_Question(char* buffer);

#endif