#ifndef TIMER_H
#define TIMER_H

#include <stdlib.h>
#include <sys/time.h>

struct timer 
{
    struct timeval last;
    int interval; // ms
};

void timer_init();
void timer_update();
int timer_interval();
void timer_reset(void);

#endif
