#ifndef GRAPHIC_H
#define GRAPHIC_H

extern int PGRD_LEFT;
extern int PGRD_TOP;
extern int PRV_LEFT;
extern int PRV_TOP;
extern int ST_LEFT;
extern int ST_TOP;
extern int LR_LEFT;
extern int LR_TOP;
extern int BR_LEFT;
extern int BR_TOP;

#define F_BLACK 30
#define F_RED 31
#define F_GREEN 32
#define F_YELLOW 33
#define F_BLUE 34
#define F_MAGENTA 35
#define F_CYAN 36
#define F_WHITE 37

#define B_BLACK 40
#define B_RED 41
#define B_GREEN 42
#define B_YELLOW 43
#define B_BLUE 44
#define B_MAGENTA 45
#define B_CYAN 46
#define B_WHITE 47

#define T_BOLD 1
#define T_UNDERSCORE 4
#define T_BLINK 5
#define T_REVERSE 7

void setattr(int i);
void setcolor(int f, int b);
void restore();

void erase_display();
void erase_line();

void center();

#endif
