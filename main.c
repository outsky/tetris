#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include "graphic.h"
#include "control.h"
#include "timer.h"
#include "tetris.h"

static void sigint(int);
static void quit(void);

static void draw(void);
static void draw_playgrd(void);
static void draw_preview(void);
static void draw_linerecord(void);
static void draw_blockrecord(void);
static void draw_status(void);

static void draw_block(int preview, int n);
static void draw_cur(void);

static void* trd_draw(void*);
static void* trd_timer(void*);

struct termios org;
int main()
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = sigint;
    sigaction(SIGINT, &sa, NULL);
    atexit(quit);
    prepare_input(&org);
    erase_display();

    srand(time(NULL));
    game_init();
    draw();

    pthread_mutex_init(&mut, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_t tid;
    pthread_create(&tid, NULL, trd_timer, NULL);
    pthread_detach(tid);
    pthread_create(&tid, NULL, trd_draw, NULL);
    pthread_detach(tid);

    int c;
    while(EOF != (c=getchar())) {
        if(c == 'j') {
            move_down();
        } else if(c == 'k') {
            rotate();
        } else if(c == 'h') {
            move_left();
        } else if(c == 'l') {
            move_right();
        } else if(c == 32) {
            drop_down();
        } else if(c == 'q') {
            quit();
        } else {
            continue;
        }
    }

    return 0;
}

static void quit(void)
{
    restore();
    restore_input(&org);
    erase_display();
    pthread_mutex_destroy(&mut);
    pthread_cond_destroy(&cond);
    printf("\n");
    exit(0);
}

static void draw(void)
{
    center();
    draw_playgrd();
    draw_preview();
    draw_linerecord();
    draw_blockrecord();
    draw_status();
    draw_cur();
    fflush(stdout);
}

static void draw_pad(int bc, int n)
{
    setcolor(0, bc);
    for(; n>0; --n)
        printf(" ");
    restore();
}

static void draw_block(int preview, int n)
{
    static const int bc[] = {B_BLACK, B_BLACK, B_RED, B_GREEN, B_YELLOW, B_BLUE, B_MAGENTA, B_CYAN, B_WHITE};

    static const char* s[] = {"  ", "  ", "()", "##", "$$", "{}", "<>", "&&", "[]"};

    int f = n==Z ? F_YELLOW : F_WHITE;
    int b = bc[n];
    if(1 == preview) {
        b = n==EMPTY ? B_BLUE : B_BLACK;
        f = n==EMPTY ? F_BLUE : F_BLACK;;
    }
    setcolor(f, b);
    printf("%s", s[n]);
    restore();
}

static void draw_preview(void)
{
    int l,c;
    for(l=0; l<4; ++l) {
        cursor_to(PRV_LEFT, PRV_TOP+l);
        draw_pad(B_BLUE, 1);
        for(c=0; c<4; ++c) {
            cursor_to(PRV_LEFT+c*2+1, PRV_TOP+l);
            draw_block(1, GAME->nextgrd[l][c]);
        }
        draw_pad(B_BLUE, 1);
    }
    cursor_to(PRV_LEFT, PRV_TOP+4);
    draw_pad(B_BLUE, 10);
}

static void draw_playgrd(void)
{
    int l,c;
    for(l=0; l<LINES; ++l) {
        for(c=0; c<COLS; ++c) {
            cursor_to(PGRD_LEFT+c*2, PGRD_TOP+l);
            draw_block(0, GAME->playgrd[l][c]);
        }
    }
}

static void draw_cur(void)
{
    int i;
    for(i=0; i<4; ++i) {
        if(GAME->cur[i].line < 0)
            continue;

        cursor_to(PGRD_LEFT+GAME->cur[i].col*2, PGRD_TOP+GAME->cur[i].line);
        draw_block(0, GAME->curtype);
    }
}

static void draw_linerecord(void)
{
    cursor_to(LR_LEFT, LR_TOP);
    setcolor(F_WHITE, B_BLACK);
    setattr(T_BOLD);
    printf("   LINE   ");
    restore();
    setcolor(F_BLACK, B_WHITE);
    cursor_to(LR_LEFT, LR_TOP+1);
    printf(" 1> %5d ", GAME->one);
    cursor_to(LR_LEFT, LR_TOP+2);
    printf(" 2> %5d ", GAME->two);
    cursor_to(LR_LEFT, LR_TOP+3);
    printf(" 3> %5d ", GAME->three);
    cursor_to(LR_LEFT, LR_TOP+4);
    printf(" 4> %5d ", GAME->four);
    cursor_to(LR_LEFT, LR_TOP+5);
    setattr(T_UNDERSCORE);
    printf(" sum %4d ", GAME->one+GAME->two+GAME->three+GAME->four);
    restore();
}

static void draw_blockrecord(void)
{
    cursor_to(BR_LEFT, BR_TOP);
    setcolor(F_WHITE, B_BLACK);
    setattr(T_BOLD);
    printf("  BLOCKS  ");
    restore();

    cursor_to(BR_LEFT, BR_TOP+1);
    setcolor(F_WHITE, B_RED);
    printf(" (I) %4d ", GAME->i);
    cursor_to(BR_LEFT, BR_TOP+2);
    setcolor(F_WHITE, B_GREEN);
    printf(" #J# %4d ", GAME->j);
    cursor_to(BR_LEFT, BR_TOP+3);
    setcolor(F_WHITE, B_YELLOW);
    printf(" $L$ %4d ", GAME->l);
    cursor_to(BR_LEFT, BR_TOP+4);
    setcolor(F_WHITE, B_BLUE);
    printf(" {O} %4d ", GAME->o);
    cursor_to(BR_LEFT, BR_TOP+5);
    setcolor(F_WHITE, B_MAGENTA);
    printf(" <S> %4d ", GAME->s);
    cursor_to(BR_LEFT, BR_TOP+6);
    setcolor(F_WHITE, B_CYAN);
    printf(" &T& %4d ", GAME->t);
    cursor_to(BR_LEFT, BR_TOP+7);
    setcolor(F_YELLOW, B_WHITE);
    printf(" [Z] %4d ", GAME->z);
    cursor_to(BR_LEFT, BR_TOP+8);
    setcolor(F_WHITE, B_BLACK);
    setattr(T_UNDERSCORE);
    printf(" sum %4d ", GAME->i+GAME->j+GAME->l+GAME->o+GAME->s+GAME->t+GAME->z);
    restore();
}

static void draw_status(void)
{
    cursor_to(ST_LEFT, ST_TOP);
    setcolor(F_WHITE, B_BLACK);
    setattr(T_BOLD);
    printf("  LEVEL  ");
    restore();
    setcolor(F_BLACK, B_WHITE);
    cursor_to(ST_LEFT, ST_TOP+1);
    printf("%8d ", GAME->level+1);

    cursor_to(ST_LEFT, ST_TOP+3);
    setcolor(F_WHITE, B_BLACK);
    setattr(T_BOLD);
    printf("  SCORE  ");
    restore();
    setcolor(F_BLACK, B_WHITE);
    cursor_to(ST_LEFT, ST_TOP+4);
    printf("%8d ", GAME->score);

    cursor_to(ST_LEFT, ST_TOP+6);
    setcolor(F_WHITE, B_BLACK);
    setattr(T_BOLD);
    printf("  SPEED  ");
    restore();
    setcolor(F_BLACK, B_WHITE);
    cursor_to(ST_LEFT, ST_TOP+7);
    printf(" %7.2f ", 1000.0f/speeds[GAME->level]);

    cursor_to(ST_LEFT, ST_TOP+9);
    setcolor(F_WHITE, B_BLACK);
    setattr(T_BOLD);
    printf("NEXTLEVEL");
    restore();
    setcolor(F_BLACK, B_WHITE);
    cursor_to(ST_LEFT, ST_TOP+10);
    printf("%8d ", levels[GAME->level]-GAME->score);

    restore();
}

static void* trd_draw(void* p)
{
    for(;;) {
        if(0 == pthread_mutex_lock(&mut)) {
            pthread_cond_wait(&cond, &mut);
            draw();
            pthread_mutex_unlock(&mut);
        }
    }
}
static void* trd_timer(void* p)
{
    timer_init();
    for(;;) {
        timer_update();
        if(timer_interval() >= speeds[GAME->level]) {
            move_down();
            timer_reset();
        }
        usleep(50);
    }
}

static void sigint(int n)
{
    quit();
    exit(0);
}
