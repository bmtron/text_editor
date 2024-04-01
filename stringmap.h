#include "struct_defs.h"

#ifndef STRINGMAP
#define STRINGMAP

struct StringMap * get_last_character_in_row(int row, struct StringMap *strmap);
struct StringMap * get_character_at_cursor(struct Cursor *cursor, struct StringMap *strmap);

#endif
