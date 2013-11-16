#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include "graphic.h"
#include "tetris.h"

int PGRD_LEFT = 10;
int PGRD_TOP = 5;
int PRV_LEFT = 0;
int PRV_TOP = 0;
int ST_LEFT = 0;
int ST_TOP = 0;
int LR_LEFT = 0;
int LR_TOP = 0;
int BR_LEFT = 0;
int BR_TOP = 0;

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
    
    PGRD_LEFT = (ws.ws_col-COLS*2)/2;
    PGRD_TOP = (ws.ws_row-LINES)/2;
    PRV_LEFT = PGRD_LEFT+COLS*2+1;
    PRV_TOP = PGRD_TOP;
    ST_LEFT = PGRD_LEFT-9;
    ST_TOP = PGRD_TOP;
    BR_LEFT = PRV_LEFT;
    BR_TOP = PGRD_TOP+LINES-9;
    LR_LEFT = PRV_LEFT;
    LR_TOP = BR_TOP-6;

    memcpy(&old, &ws, sizeof(struct winsize));
    erase_display();
}
