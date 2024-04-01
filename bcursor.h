#include <curses.h>
#include "struct_defs.h"
#ifndef B_CURSOR
#define B_CURSOR


void update_cursor_position(struct Cursor *cursor,
                            struct StringMap *str_map_buf, int x, int y);
#endif
