#include "graphic.h"
#include <stdio.h>

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
