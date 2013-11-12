#include "timer.h"

static struct timer* TIMER = NULL;

void timer_init()
{
    TIMER = malloc(sizeof(struct timer));
    gettimeofday(&TIMER->last, NULL);
    TIMER->interval = 0;
}

void timer_update()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    TIMER->interval = (now.tv_sec-TIMER->last.tv_sec)*0 + (now.tv_usec-TIMER->last.tv_usec)/1000;
    TIMER->last = now;
}

int timer_interval()
{
    return TIMER->interval;
}
