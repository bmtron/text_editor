
#include "bcursor.h"
#include "charlist.h"
#include <curses.h>

void update_cursor_position(struct Cursor *cursor,
                            struct CharNode *char_list_head, int x, int y) {
  if (x < cursor->x) { // backspace entered
      if (x < 0 && y > 0) {
      struct CharNode *last_char = get_last_character_in_row(y, char_list_head);
      cursor->x = 0;//last_char->prev_char_col-2;
      cursor->y = y - 1;
    }
    if (x < 0 && y == 0) {
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
