#include <stdio.h>
#include "control.h"

void cursor_to(int line, int col)
{
    printf("\033[%d;%dH", col, line);
}

void cursor_up(int n)
{
    printf("\033[%dA", n);
}

void cursor_down(int n)
{
    printf("\033[%dB", n);
}

void cursor_left(int n)
{
    printf("\033[%dD", n);
}

void cursor_right(int n)
{
    printf("\033[%dC", n);
}

void cursor_save()
{
    printf("\033[s");
}

void cursor_restore()
{
    printf("\033[u");
}

void cursor_hide()
{
    printf("\033[?25l");
}

void cursor_show()
{
    printf("\033[?25h");
}

void prepare_input(struct termios* org)
{
    struct termios tm;
    tcgetattr(0, org);
    tm = *org;
    tm.c_lflag &= (~ICANON);
    tm.c_lflag &= (~(ECHO|ECHOE|ECHOK|ECHONL));
    tm.c_cc[VTIME] = 0;
    tm.c_cc[VMIN] = 1;
    tcsetattr(0, TCSANOW, &tm);

    cursor_hide();
}

void restore_input(const struct termios* org)
{
    tcsetattr(0, TCSANOW, org);
    cursor_show();
}

