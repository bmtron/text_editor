#include "stringmap.h"
#include <stdio.h>

struct StringMap * get_last_character_in_row(int row, struct StringMap *strmap) {
    struct StringMap *tmp_map_ptr = NULL;

    while (strmap->row != row) {
        strmap++;
    }

    tmp_map_ptr = strmap;
    while (tmp_map_ptr->data) {
        tmp_map_ptr++;
    }

    return tmp_map_ptr;
}

struct StringMap * get_character_at_cursor(struct Cursor *cursor, struct StringMap *strmap) {
    return &strmap[cursor->x + cursor->y];
}
