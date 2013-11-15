#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include "graphic.h"
#include "tetris.h"

int PGRD_PADLEFT = 10;
int PGRD_PADTOP = 5;
int PRV_PADLEFT = 0;
int PRV_PADTOP = 0;
int INF_PADLEFT = 0;
int INF_PADTOP = 0;

void setattr(int i)
{
    printf("\033[%dm", i);
}

void setcolor(int f, int b)
{
    printf("\033[%d;%dm", f, b);
}

void restore()
{
    setattr(0);
}

void erase_display()
{
    printf("\033[2J");
}

void erase_line()
{
    printf("\033[K");
}

void center()
{
    static struct winsize old;
    struct winsize ws;
    if(-1==ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) ||
      0==memcmp(&old, &ws, sizeof(struct winsize)))
        return;
    
    PGRD_PADLEFT = (ws.ws_col-COLS*2)/2;
    PGRD_PADTOP = (ws.ws_row-LINES)/2;
    PRV_PADLEFT = PGRD_PADLEFT+COLS*2;
    PRV_PADTOP = PGRD_PADTOP;
    INF_PADLEFT = PRV_PADLEFT;
    INF_PADTOP = PRV_PADTOP+4;

    memcpy(&old, &ws, sizeof(struct winsize));
    erase_display();
}
