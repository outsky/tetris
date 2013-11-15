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
static void draw_preview(void);
static void draw_playgrd(void);
static void draw_cur(void);
static void draw_info(void);
static void draw_block(int preview, int n);

static void* trd_input(void*);
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
    next();

    pthread_t tid;
    pthread_create(&tid, NULL, trd_input, NULL);
    pthread_detach(tid);
    pthread_create(&tid, NULL, trd_timer, NULL);
    pthread_detach(tid);
    for(;;) {
        draw();
        fflush(stdout);
        sleep(1);
    }


    return 0;
}

static void quit(void)
{
    restore();
    restore_input(&org);
    erase_display();
    printf("\n");
    exit(0);
}

static void draw(void)
{
    center();
    draw_playgrd();
    draw_preview();
    draw_info();
    draw_cur();
}

static void draw_block(int preview, int n)
{
    int f,b;
    f = 1==preview ? 35 : 34;
    char* s;
    switch(n)
    {
        case 0:
        b = 1==preview ? 41 : 40;
        s = "_|";
        break;

        case 1:
        b = 41;
        s = "  ";
        break;

        case 2:
        b = 42;
        s = "  ";
        break;

        case 3:
        b = 43;
        s = "  ";
        break;

        case 4:
        b = 44;
        s = "  ";
        break;

        case 5:
        b = 45;
        s = "  ";
        break;

        case 6:
        b = 46;
        s = "  ";
        break;

        case 7:
        b = 43;
        s = "  ";
        break;

        case 8:
        b = 44;
        s = "  ";
        break;

        default:
        return;
    }
    setcolor(f, b);
    printf("%s", s);
    restore();
}

static void draw_preview(void)
{
    int l,c;
    for(l=0; l<4; ++l) {
        for(c=0; c<4; ++c) {
            cursor_to(PRV_PADLEFT+c*2, PRV_PADTOP+l);
            draw_block(1, GAME->nextgrd[l][c]);
        }
    }
}

static void draw_playgrd(void)
{
    int l,c;
    for(l=0; l<LINES; ++l) {
        for(c=0; c<COLS; ++c) {
            cursor_to(PGRD_PADLEFT+c*2, PGRD_PADTOP+l);
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

        cursor_to(PGRD_PADLEFT+GAME->cur[i].col*2, PGRD_PADTOP+GAME->cur[i].line);
        draw_block(0, GAME->curtype);
    }
}

static void draw_info(void)
{
    setcolor(33, 42);
    setattr(1);
    cursor_to(INF_PADLEFT, INF_PADTOP);
    printf("        ");
    cursor_to(INF_PADLEFT, INF_PADTOP+1);
    printf("Lv.%5d", GAME->level+1);
    cursor_to(INF_PADLEFT, INF_PADTOP+2);
    printf("        ");

    setcolor(32, 44);
    cursor_to(INF_PADLEFT, INF_PADTOP+3);
    printf("%8d", GAME->score);

    setcolor(34, 40);
    cursor_to(INF_PADLEFT, INF_PADTOP+4);
    printf("%8d", GAME->lines);

    setcolor(37, 40);
    cursor_to(INF_PADLEFT, INF_PADTOP+5);
    printf("%8d", GAME->one);

    cursor_to(INF_PADLEFT, INF_PADTOP+6);
    printf("%8d", GAME->two);

    cursor_to(INF_PADLEFT, INF_PADTOP+7);
    printf("%8d", GAME->three);

    cursor_to(INF_PADLEFT, INF_PADTOP+8);
    printf("%8d", GAME->four);
    setcolor(37, 40);
    cursor_to(INF_PADLEFT, INF_PADTOP+9);
    printf("%8d", GAME->i);

    cursor_to(INF_PADLEFT, INF_PADTOP+10);
    printf("%8d", GAME->j);

    cursor_to(INF_PADLEFT, INF_PADTOP+11);
    printf("%8d", GAME->o);

    cursor_to(INF_PADLEFT, INF_PADTOP+12);
    printf("%8d", GAME->l);

    cursor_to(INF_PADLEFT, INF_PADTOP+13);
    printf("%8d", GAME->s);

    cursor_to(INF_PADLEFT, INF_PADTOP+14);
    printf("%8d", GAME->t);

    cursor_to(INF_PADLEFT, INF_PADTOP+15);
    printf("%8d", GAME->z);

    restore();
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
static void* trd_input(void* p)
{
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
        } else if(c == 'q'){
            quit();
        } else {
            continue;
        }
        draw();
    }
}

static void sigint(int n)
{
    quit();
    exit(0);
}
