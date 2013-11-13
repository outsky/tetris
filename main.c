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
static void draw_block(int n);

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
    exit(0);
}

static void draw(void)
{
    draw_playgrd();
    draw_preview();
    draw_info();
    draw_cur();
}

static void draw_block(int n)
{
    int f,b;
    f = 34;
    char* s;
    switch(n)
    {
        case 0:
        b = 40;
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
            cursor_to(10+c*2, 5+l);
            draw_block(GAME->nextgrd[l][c]);
        }
    }
}

static void draw_playgrd(void)
{
    #define PADLEFT 20
    #define PADTOP 10
    int l,c;
    for(l=0; l<LINES; ++l) {
        for(c=0; c<COLS; ++c) {
            cursor_to(PADLEFT+c*2, PADTOP+l);
            draw_block(GAME->playgrd[l][c]);
        }
    }
}

static void draw_cur(void)
{
    int i;
    for(i=0; i<4; ++i) {
        if(GAME->cur[i].line < 0)
            continue;

        cursor_to(PADLEFT+GAME->cur[i].col*2, PADTOP+GAME->cur[i].line);
        draw_block(GAME->curtype);
    }
}

static void draw_info(void)
{

}

static void* trd_timer(void* p)
{
    timer_init();
    for(;;) {
        timer_update();
        if(timer_interval() >= 1000) {
            //move_down();
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
