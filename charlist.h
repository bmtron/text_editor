#include "struct_defs.h"
#ifndef CHARLIST
#define CHARLIST
void remove_node_at_position(struct CharNode *char_list_head, int position);
struct CharNode * get_last_character_in_row(int row, struct CharNode *char_list_head);
#endif
