#ifndef B_CURSOR
#define B_CURSOR

#include <curses.h>

struct Cursor {
    int x;
    int y;
};
void update_cursor_position(struct Cursor *cursor, int x, int y);
#endif
