#ifndef CONTROL_H
#define CONTROL_H

#include <termios.h>

void cursor_to(int line, int col);
void cursor_up(int n);
void cursor_down(int n);
void cursor_left(int n);
void cursor_right(int n);

void cursor_hide();
void cursor_show();
void cursor_save();
void cursor_restore();

// for buff problems
void prepare_input(struct termios* org);
void restore_input(const struct termios* org);

#endif
