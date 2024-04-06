#ifndef STRUCT_DEFS
#define STRUCT_DEFS

struct StringMap {
    int col;
    int row;
    char data;
};
struct Cursor {
  int x;
  int y;
};
struct LList {
    char val;
    struct LList *next;
};
struct CharNode {
    char val;
    int pos;
    int row;
    int prev_char_col;
    int col;
    struct CharNode *next;
};
#endif
