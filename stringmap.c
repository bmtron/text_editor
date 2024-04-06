#include "charlist.h"
#include <stdio.h>


struct StringMap * get_character_at_cursor(struct Cursor *cursor, struct StringMap *strmap) {
    return &strmap[cursor->x + cursor->y];
}
