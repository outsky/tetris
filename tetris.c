#include "tetris.h"
#include <stdlib.h>
#include <string.h>

struct game* GAME = NULL;
#define MAXLEVEL 11
int levels[MAXLEVEL] = {25, 50, 100, 180, 280, 400, 550, 700, 900, 1200, 1500};
static int scores[4] = {1, 5, 10, 20};
int speeds[MAXLEVEL] = {1000, 950, 900, 850, 800, 750, 700, 650, 600, 550, 500};

void game_init()
{
    GAME = malloc(sizeof(struct game));
    memset(GAME, 0, sizeof(struct game));
    GAME->nexttyp = rand()%7 + 2;
    GAME->nextstate = rand()%4;
    GAME->state = PLAY;
    next();
}

static int can_move_down()
{
    int i;
    for(i=0; i<4; ++i) {
        struct pos* cur = &GAME->cur[i];
        if(cur->line+1>=LINES || (cur->line+1>=0 && GAME->playgrd[cur->line+1][cur->col]!=0))
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

int drop_down(void)
{
    while(1 == move_down())
        ;
    return 0;
}

int move_down()
{
    if(1 != can_move_down()) {
        stick();
        if(1 == isgameover()) {
            gameover();
            return 0;
        }
        int ln = clearlines();
        onclearline(ln);
        next();
        return 0;
    }

    pthread_mutex_lock(&mut);
    int i;
    for(i=0; i<4; ++i) {
        GAME->cur[i].line++;
    }
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mut);

    return 1;
}
int move_right()
{
    if(1 != can_move_right())
        return 0;

    pthread_mutex_lock(&mut);
    int i;
    for(i=0; i<4; ++i) {
        GAME->cur[i].col++;
    }
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mut);

    return 1;
}
int move_left()
{
    if(1 != can_move_left())
        return 0;

    pthread_mutex_lock(&mut);
    int i;
    for(i=0; i<4; ++i) {
        GAME->cur[i].col--;
    }
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mut);

    return 1;
}
int rotate()
{
    pthread_mutex_lock(&mut);
    int ret = 0;
    switch(GAME->curtype)
    {
        case I:
        ret = rotate_i();
        break;

        case J:
        ret = rotate_j();
        break;

        case L:
        ret = rotate_l();
        break;

        case S:
        ret = rotate_s();
        break;

        case T:
        ret = rotate_t();
        break;

        case Z:
        ret = rotate_z();
        break;

        case O:
        default:
        break;
    }

    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mut);
    return ret; 
}

// FOR ALL rotate_x FUNCTIONS
// 
// c  : the center block
// num: the index of block
// {} : next state blocks
// [] : current state blocks
// <> : blocks
// #  : this block blocks the rotation
static int rotate_i(void)
{
    struct pos* cur = GAME->cur;
    int cl = cur[1].line;
    int cc = cur[1].col;
    int nextstate = (GAME->curstate+1)%4;
    switch(nextstate) {
        /*
            <#><#>{#}
            [3][2][c][0] 
                  {#}<#>
                  {#}
        */
        case 0:
        case 2:
            if(!ISEMPTY(cl-1,cc-2) || !ISEMPTY(cl-1,cc-1) || !ISEMPTY(cl-1,cc) ||
              !ISEMPTY(cl+1,cc) || !ISEMPTY(cl+1,cc+1) || !ISEMPTY(cl+2,cc))
                return 0;
            cur[0].col = cur[2].col = cur[3].col = cc;
            cur[0].line = cl-1;
            cur[2].line = cl+1;
            cur[3].line = cl+2;
            break;

        /*
                [0]<#>
          {#}{#}[c]{#}
             <#>[2]
             <#>[3]
        */
        case 1:
        case 3:
            if(!ISEMPTY(cl-1,cc+1) || !ISEMPTY(cl+1,cc-1) || !ISEMPTY(cl+2,cc-1) ||
              !ISEMPTY(cl,cc-2) || !ISEMPTY(cl,cc-1) || !ISEMPTY(cl,cc+1)) 
                return 0;
            cur[0].line = cur[2].line = cur[3].line = cl;
            cur[0].col = cc+1;
            cur[2].col = cc-1;
            cur[3].col = cc-2;
            break;
    }
    GAME->curstate = nextstate;
    return 1;
}
static int rotate_j(void)
{
    struct pos* cur = GAME->cur;
    int cl = GAME->cur[1].line;
    int cc = GAME->cur[1].col;
    int nextstate = (GAME->curstate+1)%4;
    switch(nextstate) {
        /*
             <#>{#}
             [0][c][2]
             {#}{#}[3]
        */       
        case 0:
            if(!ISEMPTY(cl-1,cc-1) || !ISEMPTY(cl-1,cc) ||
              !ISEMPTY(cl+1,cc-1) || !ISEMPTY(cl+1,cc))
                return 0;
            cur[0].col = cur[2].col = cc;
            cur[3].col = cc-1;
            cur[0].line = cl-1;
            cur[2].line = cur[3].line = cl+1;
            break;

        /*
             {#}[0]<#>
             {#}[c]{#}
             [3][2]
        */
        case 1:
            if(!ISEMPTY(cl-1,cc-1) || !ISEMPTY(cl-1,cc+1) ||
              !ISEMPTY(cl,cc-1) || !ISEMPTY(cl,cc+1))
                return 0;
            cur[0].line = cur[2].line = cl;
            cur[3].line = cl-1;
            cur[0].col = cc+1;
            cur[2].col = cur[3].col = cc-1;
            break;
        
        /*
             [3]{#}{#}
             [2][c][0]
                {#}<#>
        */
        case 2:
            if(!ISEMPTY(cl-1,cc) || !ISEMPTY(cl-1,cc+1) ||
              !ISEMPTY(cl+1,cc) || !ISEMPTY(cl+1,cc+1))
                return 0;
            cur[0].col = cur[2].col = cc;
            cur[3].col = cc+1;
            cur[0].line = cl+1;
            cur[2].line = cur[3].line = cl-1;
            break;
        
        /*
                [2][3]
             {#}[c]{#}
             <#>[0]{#}
        */
        case 3:
            if(!ISEMPTY(cl,cc-1) || !ISEMPTY(cl,cc+1) ||
              !ISEMPTY(cl+1,cc-1) || !ISEMPTY(cl+1,cc+1))
                return 0;
            cur[0].line = cur[2].line = cl;
            cur[3].line = cl+1;
            cur[0].col = cc-1;
            cur[2].col = cur[3].col = cc+1;
            break;
    }
    GAME->curstate = nextstate;
    return 1;
}
static int rotate_l(void)
{
    struct pos* cur = GAME->cur;
    int cl = GAME->cur[1].line;
    int cc = GAME->cur[1].col;
    int nextstate = (GAME->curstate+1)%4;
    switch(nextstate)
    {
        /*
             <#>{#}[3]
             [0][c][2]
                {#}{#}
        */
        case 0:
            if(!ISEMPTY(cl-1,cc-1) || !ISEMPTY(cl-1,cc) ||
              !ISEMPTY(cl+1,cc) || !ISEMPTY(cl+1,cc+1))
                return 0;
            cur[0].col = cur[2].col = cc;
            cur[3].col = cc+1;
            cur[0].line = cl-1;
            cur[2].line = cur[3].line = cl+1;
            break;
       
        /*
                [0]<#>
             {#}[c]{#}
             {#}[2][3]
        */
        case 1:
            if(!ISEMPTY(cl-1,cc+1) || !ISEMPTY(cl,cc+1) ||
              !ISEMPTY(cl,cc-1) || !ISEMPTY(cl+1,cc-1))
                return 0;
            cur[0].line = cur[2].line = cl;
            cur[3].line = cl+1;
            cur[0].col = cc+1;
            cur[2].col = cur[3].col = cc-1;
            break;
        
        /*
             {#}{#}
             [2][c][0]
             [3]{#}<#>
        */
        case 2:
            if(!ISEMPTY(cl-1,cc-1) || !ISEMPTY(cl-1,cc) ||
              !ISEMPTY(cl+1,cc) || !ISEMPTY(cl+1,cc+1))
                return 0;
            cur[0].col = cur[2].col = cc;
            cur[3].col = cc-1;
            cur[0].line = cl+1;
            cur[2].line = cur[3].line = cl-1;
            break;
        
        /*
             [3][2]{#}
             {#}[c]{#}
             <#>[0]
        */
        case 3:
            if(!ISEMPTY(cl-1,cc+1) || !ISEMPTY(cl,cc+1) ||
              !ISEMPTY(cl,cc-1) || !ISEMPTY(cl+1,cc-1))
                return 0;
            cur[0].line = cur[2].line = cl;
            cur[3].line = cl-1;
            cur[0].col = cc-1;
            cur[2].col = cur[3].col = cc+1;
            break;
    }
    GAME->curstate = nextstate;
    return 1;
}
static int rotate_s(void)
{
    struct pos* cur = GAME->cur;
    int cl = GAME->cur[1].line;
    int cc = GAME->cur[1].col;
    int nextstate = (GAME->curstate+1)%4;
    switch(nextstate)
    {
        /*
             [3]{#}{#}
             [2][c]
             <#>[0]
        */
        case 0:
        case 2:
            if(!ISEMPTY(cl-1,cc) || !ISEMPTY(cl-1,cc+1) || !ISEMPTY(cl+1,cc-1))
                return 0;
            cur[0].col = cc+1;
            cur[2].col = cc;
            cur[3].col = cc-1;
            cur[0].line = cl;
            cur[2].line = cur[3].line = cl+1;
            break;
       
        /*
             {#}
             {#}[c][0]
             [3][2]<#>
        */
        case 1:
        case 3:
            if(!ISEMPTY(cl-1,cc-1) || !ISEMPTY(cl,cc-1) || !ISEMPTY(cl+1,cc+1))
                return 0;
            cur[0].col = cc;
            cur[2].col = cur[3].col = cc-1;
            cur[0].line = cl+1;
            cur[2].line = cl;
            cur[3].line = cl-1;
            break;
    }
    GAME->curstate = nextstate;
    return 1;
}
static int rotate_t(void)
{
    struct pos* cur = GAME->cur;
    int cl = GAME->cur[1].line;
    int cc = GAME->cur[1].col;
    int nextstate = (GAME->curstate+1)%4;
    switch(nextstate)
    {
        /*
                [2]<#>
             {#}[c][3]
             <#>[0]<#>
        */
        case 0:
            if(!ISEMPTY(cl,cc-1) || !ISEMPTY(cl+1,cc-1) ||
              !ISEMPTY(cl-1,cc+1) || !ISEMPTY(cl+1,cc+1))
                return 0;
            cur[0].col = cc-1;
            cur[2].col = cc+1;
            cur[3].col = cc;
            cur[0].line = cur[2].line = cl;
            cur[3].line = cl+1;
            break;
       
        /*
             <#>{#}
             [0][c][2]
             <#>[3]<#>
        */
        case 1:
            if(!ISEMPTY(cl-1,cc-1) || !ISEMPTY(cl-1,cc) ||
              !ISEMPTY(cl+1,cc-1) || !ISEMPTY(cl+1,cc+1))
                return 0;
            cur[0].col = cur[2].col = cc;
            cur[3].col = cc-1;
            cur[0].line = cl-1;
            cur[2].line = cl+1;
            cur[3].line = cl;
            break;
        
        /*
             <#>[0]<#>
             [3][c]{#}
             <#>[2]
        */
        case 2:
            if(!ISEMPTY(cl-1,cc-1) || !ISEMPTY(cl+1,cc-1) ||
              !ISEMPTY(cl-1,cc+1) || !ISEMPTY(cl,cc+1))
                return 0;
            cur[0].col = cc+1;
            cur[2].col = cc-1;
            cur[3].col = cc;
            cur[0].line = cur[2].line = cl;
            cur[3].line = cl-1;
            break;
        
        /*
             <#>[3]<#>
             [2][c][0]
                {#}<#>
        */
        case 3:
            if(!ISEMPTY(cl-1,cc-1) || !ISEMPTY(cl-1,cc+1) ||
              !ISEMPTY(cl+1,cc) || !ISEMPTY(cl+1,cc+1))
                return 0;
            cur[0].col = cur[2].col = cc;
            cur[3].col = cc+1;
            cur[0].line = cl+1;
            cur[2].line = cl-1;
            cur[3].line = cl;
            break;
    }
    GAME->curstate = nextstate;
    return 1;
}
static int rotate_z(void)
{
    struct pos* cur = GAME->cur;
    int cl = GAME->cur[1].line;
    int cc = GAME->cur[1].col;
    int nextstate = (GAME->curstate+1)%4;
    switch(nextstate)
    {
        /*
             {#}[0]<#>
             [2][c]{#}
             [3]
        */
        case 0:
        case 2:
            if(!ISEMPTY(cl-1,cc-1) || !ISEMPTY(cl-1,cc+1) || !ISEMPTY(cl,cc+1))
                return 0;
            cur[0].col = cc-1;
            cur[2].col = cc;
            cur[3].col = cc+1;
            cur[0].line = cl;
            cur[2].line = cur[3].line = cl+1;
            break;
       
        /*
             <#>{#}
             [0][c]
             {#}[2][3]
        */
        case 1:
        case 3:
            if(!ISEMPTY(cl-1,cc-1) || !ISEMPTY(cl-1,cc) || !ISEMPTY(cl+1,cc-1))
                return 0;
            cur[0].col = cc;
            cur[2].col = cur[3].col = cc-1;
            cur[0].line = cl-1;
            cur[2].line = cl;
            cur[3].line = cl+1;
            break;
    }
    GAME->curstate = nextstate;
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
int clearlines(void)
{
    int from, to;
    from = to = GAME->cur[0].line;
    int i;
    for(i=1; i<4; ++i) {
        if(GAME->cur[i].line < from)
            from = GAME->cur[i].line;
        if(GAME->cur[i].line > to)
            to = GAME->cur[i].line;
    }
    int count = 0;
    for(i=from; i<=to; ++i) {
        if(1 == clearline(i))
            ++count;
    }
    return count;
}

static void next(void)
{
    static int i=0;
    GAME->curtype = GAME->nexttyp;
    GAME->curstate = GAME->nextstate;
    GAME->nexttyp = rand()%7 + 2;
    GAME->nextstate = rand()%4;
    fillnext();
    fillcur();
}

static void fillnext(void)
{
    static int pattern[7][4][16] = {
        { // I
            {0,0,2,0,
             0,0,2,0,
             0,0,2,0,
             0,0,2,0},
            {0,0,0,0,
             0,0,0,0,
             2,2,2,2,
             0,0,0,0},
            {0,0,2,0,
             0,0,2,0,
             0,0,2,0,
             0,0,2,0},
            {0,0,0,0,
             0,0,0,0,
             2,2,2,2,
             0,0,0,0}
        },{ // J
            {0,0,0,0,
             0,0,3,0,
             0,0,3,0,
             0,3,3,0},
            {0,0,0,0,
             0,0,0,0,
             0,3,0,0,
             0,3,3,3},
            {0,0,0,0,
             0,3,3,0,
             0,3,0,0,
             0,3,0,0},
            {0,0,0,0,
             0,0,0,0,
             0,3,3,3,
             0,0,0,3}
        },{ // L
            {0,0,0,0,
             0,4,0,0,
             0,4,0,0,
             0,4,4,0},
            {0,0,0,0,
             0,0,0,0,
             0,4,4,4,
             0,4,0,0},
            {0,0,0,0,
             0,4,4,0,
             0,0,4,0,
             0,0,4,0},
            {0,0,0,0,
             0,0,0,0,
             0,0,0,4,
             0,4,4,4}
        },{ // O
            {0,0,0,0,
             0,0,0,0,
             0,5,5,0,
             0,5,5,0},
            {0,0,0,0,
             0,0,0,0,
             0,5,5,0,
             0,5,5,0},
            {0,0,0,0,
             0,0,0,0,
             0,5,5,0,
             0,5,5,0},
            {0,0,0,0,
             0,0,0,0,
             0,5,5,0,
             0,5,5,0}
        },{ // S
            {0,0,0,0,
             0,0,0,0,
             0,0,6,6,
             0,6,6,0},
            {0,0,0,0,
             0,6,0,0,
             0,6,6,0,
             0,0,6,0},
            {0,0,0,0,
             0,0,0,0,
             0,0,6,6,
             0,6,6,0},
            {0,0,0,0,
             0,6,0,0,
             0,6,6,0,
             0,0,6,0}
        },{ // T
            {0,0,0,0,
             0,0,0,0,
             0,7,7,7,
             0,0,7,0},
            {0,0,0,0,
             0,0,7,0,
             0,7,7,0,
             0,0,7,0},
            {0,0,0,0,
             0,0,0,0,
             0,0,7,0,
             0,7,7,7},
            {0,0,0,0,
             0,7,0,0,
             0,7,7,0,
             0,7,0,0}
        },{ // Z
            {0,0,0,0,
             0,8,8,0,
             0,0,8,8,
             0,0,0,0},
            {0,0,0,0,
             0,8,0,0,
             0,8,8,0,
             0,0,8,0},
            {0,0,0,0,
             0,8,8,0,
             0,0,8,8,
             0,0,0,0},
            {0,0,0,0,
             0,8,0,0,
             0,8,8,0,
             0,0,8,0}
        }
    };

    int t = GAME->nexttyp;
    int s = GAME->nextstate;
    if(t>Z || t<I || s<0 || s>3)
        return;

    memcpy(GAME->nextgrd, pattern[t-2][s], sizeof(GAME->nextgrd));
}

static void fillcur(void)
{
    memset(GAME->cur, 0, sizeof(GAME->cur));
    struct pos* cur = GAME->cur;
    switch(GAME->curtype)
    {
        case I:
            GAME->i++;
            cur[0].line=-4; cur[0].col=COLS/2;      // 0
            cur[1].line=-3; cur[1].col=COLS/2;      // 1
            cur[2].line=-2; cur[2].col=COLS/2;      // 2
            cur[3].line=-1; cur[3].col=COLS/2;      // 3
            break;

        case J:
            GAME->j++;
            cur[0].line=-3; cur[0].col=COLS/2;      //
            cur[1].line=-2; cur[1].col=COLS/2;      // 0
            cur[2].line=-1; cur[2].col=COLS/2;      // 1
            cur[3].line=-1; cur[3].col=COLS/2-1;    //32
            break;

        case L:
            GAME->l++;
            cur[0].line=-3; cur[0].col=COLS/2;      // 
            cur[1].line=-2; cur[1].col=COLS/2;      // 0
            cur[2].line=-1; cur[2].col=COLS/2;      // 1
            cur[3].line=-1; cur[3].col=COLS/2+1;    // 23
            break;

        case O:
            GAME->o++;
            cur[0].line=-2; cur[0].col=COLS/2;      // 
            cur[1].line=-2; cur[1].col=COLS/2+1;    // 
            cur[2].line=-1; cur[2].col=COLS/2;      // 01
            cur[3].line=-1; cur[3].col=COLS/2+1;    // 23
            break;

        case S:
            GAME->s++;
            cur[0].line=-2; cur[0].col=COLS/2+1;    //
            cur[1].line=-2; cur[1].col=COLS/2;      //
            cur[2].line=-1; cur[2].col=COLS/2;      // 10
            cur[3].line=-1; cur[3].col=COLS/2-1;    //32
            break;

        case T:
            GAME->t++;
            cur[0].line=-2; cur[0].col=COLS/2-1;    //
            cur[1].line=-2; cur[1].col=COLS/2;      //
            cur[2].line=-2; cur[2].col=COLS/2+1;    //012 
            cur[3].line=-1; cur[3].col=COLS/2;      // 3
            break;

        case Z:
            GAME->z++;
            cur[0].line=-2; cur[0].col=COLS/2-1;    //
            cur[1].line=-2; cur[1].col=COLS/2;      //
            cur[2].line=-1; cur[2].col=COLS/2;      //01
            cur[3].line=-1; cur[3].col=COLS/2+1;    // 23
            break;

        default:
            return;
    }

    GAME->curstate = GAME->curstate==0 ? 3 : GAME->curstate-1;
    rotate();
    settlecur();
}

static void settlecur(void)
{
    int line = GAME->cur[0].line;
    int i;
    for(i=1; i<4; ++i) {
        if(line < GAME->cur[i].line)
            line = GAME->cur[i].line;
    }
    for(; line<0; ++line)
        move_down();
}

static void stick(void)
{
    int i;
    for(i=0; i<4; ++i) {
        if(GAME->cur[i].line < 0)
            continue;
        GAME->playgrd[GAME->cur[i].line][GAME->cur[i].col] = GAME->curtype;
    }
}

static int isgameover(void)
{
    int i;
    for(i=0; i<COLS; ++i) {
        if(GAME->playgrd[0][i] != 0)
            return 1;
    }
    return 0;
}

static void gameover(void)
{
    GAME->state = OVER;
}

static void onclearline(int n)
{
    if(n<=0 || n>4)
        return;
    switch(n) {
        case 1:
            GAME->one++;
            break;
        case 2:
            GAME->two++;
            break;
        case 3:
            GAME->three++;
            break;
        case 4:
            GAME->four++;
            break;
    }
    GAME->score += scores[n-1];

    if(GAME->level<MAXLEVEL && GAME->score>levels[GAME->level])
        GAME->level++;
}
