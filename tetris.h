#ifndef TETRIS_H
#define TETRIS_H

#include <pthread.h>

#define LINES 20
#define COLS 10

#define READY   0
#define PLAY    1
#define PAUSE   2
#define OVER    3

#define EMPTY 0
#define ACTIVE 1
#define I 2
#define J 3
#define L 4
#define O 5
#define S 6
#define T 7
#define Z 8

#define ISEMPTY(line, col) (((col)>=0&&(col)<COLS) && ((line)<0 || ((line)<LINES&&GAME->playgrd[line][col]==EMPTY)))

extern int speeds[];
extern int levels[];

struct pos
{
    int line;
    int col;
};

struct game
{
    int state;
    int curtype;
    int curstate;
    struct pos cur[4];
    int playgrd[LINES][COLS]; // 0(empty); 1(active); 2(I); 3(J); 4(L); 5(O); 6(S); 7(T); 8(Z)
    int nextgrd[4][4];
    int nexttyp;
    int nextstate;
    int score;
    int level;
    int one,two,three,four;
    int i,j,l,o,s,t,z;
};

extern struct game* GAME;

void game_init();

static int can_move_down();
static int can_move_left();
static int can_move_right();

int move_down();
int move_right();
int move_left();
int rotate();
int drop_down(void);

static int rotate_i(void);
static int rotate_j(void);
static int rotate_l(void);
static int rotate_s(void);
static int rotate_t(void);
static int rotate_z(void);

static int islinefull(int n);
static int clearline(int n);
int clearlines(void);

static void next(void);
static void fillnext(void);
static void fillcur(void);
static void settlecur(void);
static void stick(void);
static void gameover(void);
static int isgameover(void);

static void onclearline(int n);

pthread_cond_t cond;
pthread_mutex_t mut;

#endif
