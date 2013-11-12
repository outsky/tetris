#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <pthread.h>
#include "graphic.h"
#include "control.h"
#include "timer.h"
#include "tetris.h"

static void quit(void);

static void draw(void);
static void draw_preview(void);
static void draw_playgrd(void);
static void draw_cur(void);
static void draw_block(int n);

static void* trd_input(void*);

struct termios org;
int main()
{
    atexit(quit);
    prepare_input(&org);
    erase_display();
    setcolor(33, 40);

    srand(time(NULL));
    game_init();
    next();

    pthread_t tid;
    pthread_create(&tid, NULL, trd_input, NULL);
    pthread_join(tid, NULL);
    draw();
    /*
    for(;;) {
        draw();
        sleep(2);
    }
    */


    return 0;
}

static void quit(void)
{
    restore();
    restore_input(&org);
    erase_display();
    printf("quit\n");
}

static void draw(void)
{
    erase_display();
    draw_preview();
    draw_playgrd();
    draw_cur();
}

static void draw_block(int n)
{
    int f,b;
    f = 30;
    char* s;
    switch(n)
    {
        case 0:
        b = 47;
        s = "[0";
        break;

        case 1:
        b = 41;
        s = "[1";
        break;

        case 2:
        b = 42;
        s = "[2";
        break;

        case 3:
        b = 43;
        s = "[3";
        break;

        case 4:
        b = 44;
        s = "[4";
        break;

        case 5:
        b = 45;
        s = "[5";
        break;

        case 6:
        b = 46;
        s = "[6";
        break;

        case 7:
        b = 43;
        s = "[7";
        break;

        case 8:
        b = 44;
        s = "[8";
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
            exit(0);
        } else {
            continue;
        }
        draw();
    }
}
