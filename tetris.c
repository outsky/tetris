#include "tetris.h"
#include <stdlib.h>
#include <string.h>

struct game* GAME = NULL;

void game_init()
{
    GAME = malloc(sizeof(struct game));
    memset(GAME, 0, sizeof(struct game));
    next();
    next();
}

static int can_move_down()
{
    int i;
    for(i=0; i<4; ++i) {
        struct pos* cur = &GAME->cur[i];
        if(cur->line+1>=LINES || (cur->line>=0 && GAME->playgrd[cur->line+1][cur->col]!=0))
            return 0;
    }
    return 1;
}
static int can_move_left()
{
    int i;
    for(i=0; i<4; ++i) {
        struct pos* cur = &GAME->cur[i];
        if(cur->col-1<0 || (cur->line>=0 && GAME->playgrd[cur->line][cur->col-1]!=0))
            return 0;
    }
    return 1;
}
static int can_move_right()
{
    int i;
    for(i=0; i<4; ++i) {
        struct pos* cur = &GAME->cur[i];
        if(cur->col+1>=COLS || (cur->line>=0 && GAME->playgrd[cur->line][cur->col+1]!=0))
            return 0;
    }
    return 1;
}
static int can_rotate()
{
    return 0;
}

int move_down()
{
    if(1 != can_move_down()) {
        next();
        return 0;
    }

    int i;
    for(i=0; i<4; ++i) {
        GAME->cur[i].line++;
    }
    return 1;
}
int move_right()
{
    if(1 != can_move_right())
        return 0;

    int i;
    for(i=0; i<4; ++i) {
        GAME->cur[i].col++;
    }
    return 1;
}
int move_left()
{
    if(1 != can_move_left())
        return 0;

    int i;
    for(i=0; i<4; ++i) {
        GAME->cur[i].col--;
    }
    return 1;
}
int rotate()
{
    if(1 != can_rotate())
        return 0;

    return 1;
}

static int islinefull(int n)
{
    int i;
    for(i=0; i<COLS; ++i) {
        if(0 == GAME->playgrd[n][i])
            return 0;
    }
    return 1;
}
static int clearline(int n)
{
    if(1 == islinefull(n)) {
        int i;
        for(i=n; i>0; --i)
            memcpy(GAME->playgrd[i], GAME->playgrd[i-1], COLS*sizeof(int));
        memset(GAME->playgrd[0], 0, COLS*sizeof(int));
        return 1;
    }
    return 0;
}
int clearlines(int from, int to)
{
    int count = 0;
    int i;
    for(i=from; i<=to; ++i) {
        if(1 == clearline(i))
            ++count;
    }
    return count;
}

void next()
{
    GAME->curtype = GAME->nexttyp;
    GAME->curstate = GAME->nextstate;
    GAME->nexttyp = rand()%7 + 2;
    GAME->nextstate = rand()%4;
    fillnext();
    fillcur();
}

static void fillnext(void)
{
    memset(GAME->nextgrd, 0, sizeof(GAME->nextgrd));
    int (*nextgrd)[4] = GAME->nextgrd;
    switch(GAME->nexttyp)
    {
        case 2:
            nextgrd[0][2] = nextgrd[1][2] = nextgrd[2][2] = nextgrd[3][2] = 2;
            break;

        case 3:
            nextgrd[0][2] = nextgrd[1][2] = nextgrd[2][2] = nextgrd[2][1] = 3;
            break;

        case 4:
            nextgrd[0][2] = nextgrd[1][2] = nextgrd[2][2] = nextgrd[2][3] = 4;
            break;

        case 5:
            nextgrd[1][1] = nextgrd[1][2] = nextgrd[2][1] = nextgrd[2][2] = 5;
            break;

        case 6:
            nextgrd[2][0] = nextgrd[2][1] = nextgrd[1][1] = nextgrd[1][2] = 6;
            break;

        case 7:
            nextgrd[1][1] = nextgrd[1][2] = nextgrd[1][3] = nextgrd[2][2] = 7;
            break;

        case 8:
            nextgrd[1][1] = nextgrd[1][2] = nextgrd[2][2] = nextgrd[2][3] = 8;
            break;

        default:
            return;
    }
}

static void fillcur(void)
{
    memset(GAME->cur, 0, sizeof(GAME->cur));
    struct pos* cur = GAME->cur;
    switch(GAME->curtype)
    {
        case 2:
            cur[0].line=-4; cur[0].col=COLS/2;
            cur[1].line=-3; cur[1].col=COLS/2;
            cur[2].line=-2; cur[2].col=COLS/2;
            cur[3].line=-1; cur[3].col=COLS/2;
            break;

        case 3:
            cur[0].line=-3; cur[0].col=COLS/2;
            cur[1].line=-2; cur[1].col=COLS/2;
            cur[2].line=-1; cur[2].col=COLS/2;
            cur[3].line=-1; cur[3].col=COLS/2-1;
            break;

        case 4:
            cur[0].line=-3; cur[0].col=COLS/2;
            cur[1].line=-2; cur[1].col=COLS/2;
            cur[2].line=-1; cur[2].col=COLS/2;
            cur[3].line=-1; cur[3].col=COLS/2+1;
            break;

        case 5:
            cur[0].line=-2; cur[0].col=COLS/2;
            cur[1].line=-2; cur[1].col=COLS/2+1;
            cur[2].line=-1; cur[2].col=COLS/2;
            cur[3].line=-1; cur[3].col=COLS/2+1;
            break;

        case 6:
            cur[0].line=-2; cur[0].col=COLS/2+1;
            cur[1].line=-2; cur[1].col=COLS/2;
            cur[2].line=-1; cur[2].col=COLS/2;
            cur[3].line=-1; cur[3].col=COLS/2-1;
            break;

        case 7:
            cur[0].line=-2; cur[0].col=COLS/2-1;
            cur[1].line=-2; cur[1].col=COLS/2;
            cur[2].line=-2; cur[2].col=COLS/2+1;
            cur[3].line=-1; cur[3].col=COLS/2;
            break;

        case 8:
            cur[0].line=-2; cur[0].col=COLS/2-1;
            cur[1].line=-2; cur[1].col=COLS/2;
            cur[2].line=-1; cur[2].col=COLS/2;
            cur[3].line=-1; cur[3].col=COLS/2+1;
            break;

        default:
            return;
    }
}
