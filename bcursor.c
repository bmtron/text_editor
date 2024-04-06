#include "bcursor.h"
#include "charlist.h"
#include <curses.h>

void update_cursor_position(struct Cursor *cursor,
                            struct CharNode *char_list_head, int x, int y) {
  if (x < cursor->x) { // backspace entered
    if (x < 0 && y <= 0) {
      cursor->x = 0;
      cursor->y = 0;
    }
    if (x >= 0 && y == cursor->y) {
      cursor->x = x;
      cursor->y = y;
    }
  }

  if (y > cursor->y && y <= 24) {
    cursor->x = 0;
    cursor->y = y;
  }
}
