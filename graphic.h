#ifndef GRAPHIC_H
#define GRAPHIC_H

extern int PGRD_PADLEFT;
extern int PGRD_PADTOP;
extern int PRV_PADLEFT;
extern int PRV_PADTOP;
extern int INF_PADLEFT;
extern int INF_PADTOP;

void setattr(int i);
void setcolor(int f, int b);
void restore();

void erase_display();
void erase_line();

void center();

#endif
