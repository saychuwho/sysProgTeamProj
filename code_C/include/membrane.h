#ifndef MEMBRANE
#define MEMBRANE

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

void init_keypad();

char readLine(int line, char* characters);

char get_keys();



#endif
