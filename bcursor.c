
#include "bcursor.h"
#include "stringmap.h"
#include <curses.h>

void update_cursor_position(struct Cursor *cursor,
                            struct StringMap *str_map_buf, int x, int y) {
  if (x < cursor->x) { // backspace entered
    if (x < 0 && y > 0) {
      struct StringMap *strmap = get_last_character_in_row(y, str_map_buf);
      cursor->x = strmap->col;
      cursor->y = strmap->row;
    }
    if (x < 0 && y == 0) {
      cursor->x = 0;
      cursor->y = 0;
    }
    if (x > 0 && y == cursor->y) {

      cursor->x = x;
      cursor->y = y;
    }
  }

  if (y > cursor->y && y <= 24) {
    cursor->x = 0;
    cursor->y = y;
  }
}
