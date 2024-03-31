#include "bcursor.h"
#include <curses.h>

void update_cursor_position(struct Cursor *cursor, int x, int y) {
    if (x < 0 && cursor->y > 0) { //then we hit the beginning of a line, wrap back please 
            
    }
}

