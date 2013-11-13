#include "timer.h"

static struct timer* TIMER = NULL;

void timer_init()
{
    TIMER = malloc(sizeof(struct timer));
    timer_reset();
}

void timer_update()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    TIMER->interval = (now.tv_sec-TIMER->last.tv_sec)*1000 + (now.tv_usec-TIMER->last.tv_usec)/1000;
}

int timer_interval()
{
    return TIMER->interval;
}

void timer_reset(void)
{
    gettimeofday(&TIMER->last, NULL);
    TIMER->interval = 0;
}
