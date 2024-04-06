#include <curses.h>
#include "struct_defs.h"
#ifndef B_CURSOR
#define B_CURSOR


void update_cursor_position(struct Cursor *cursor,
                            struct CharNode *char_list_head, int x, int y);
#endif
