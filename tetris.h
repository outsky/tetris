#ifndef TETRIS_H
#define TETRIS_H

#define LINES 20
#define COLS 10

struct pos
{
    int line;
    int col;
};

struct game
{
    int curtype;
    int curstate;
    struct pos cur[4];
    int playgrd[LINES][COLS]; // 0(empty); 1(active); 2(I); 3(J); 4(L); 5(O); 6(S); 7(T); 8(Z)
    int nextgrd[4][4];
    int nexttyp;
    int nextstate;
    int score;
    int level;
    int lines;
    int I,J,L,O,S,T,Z;
};

extern struct game* GAME;

void game_init();

static int can_move_down();
static int can_move_left();
static int can_move_right();
static int can_rotate();

int move_down();
int move_right();
int move_left();
int rotate();

static int islinefull(int n);
static int clearline(int n);
int clearlines(int from, int to);

void next();
static void fillnext();
static void fillcur();

#endif
