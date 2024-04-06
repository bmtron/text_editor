#include "bcursor.h"
#include "charlist.h"
#include "stringmap.h"
#include "struct_defs.h"

#include <curses.h>
#include <fcntl.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_WIN_HEIGHT 24
#define MAX_WIN_WIDTH 80
#define OUTPUT_BUF_SIZE MAX_WIN_WIDTH *MAX_WIN_HEIGHT
#define STRMAP_BUF_SIZE sizeof(struct StringMap) * OUTPUT_BUF_SIZE

void run_screen_init();
void init_input_loop(WINDOW *mainwin, struct Cursor *cursor,
                     struct CharNode *char_list_head);
void handle_backspace_key(WINDOW *mainwin, struct Cursor *cursor,
                          struct CharNode *char_list_head);
void handle_enter_key(WINDOW *mainwin, struct Cursor *cursor,
                      struct CharNode *char_list_head);
void handle_character_key(WINDOW *mainwin, struct Cursor *cursor, int curr_char,
                          struct CharNode *char_list_head);
void file_output(struct CharNode *char_list_head, char *output_file_buf);

int total_char_count = 0;
int main(int argc, char **argv) {

  setlocale(LC_ALL, "");
  run_screen_init();

  WINDOW *mainwin = newwin(MAX_WIN_HEIGHT, MAX_WIN_WIDTH, 0, 0);
  wrefresh(mainwin);

  struct Cursor cursor = {.x = mainwin->_curx, .y = mainwin->_cury};
  struct CharNode *char_list_head =
      malloc(sizeof(struct CharNode)); //{ .val = '\0', .pos = 0, .row = 0, .col
                                       //= 0, .next = NULL };
  char_list_head->val = '\0';
  char_list_head->next = NULL;
  char_list_head->pos = 0;
  char_list_head->row = 0;
  char_list_head->col = 0;
  char *output_file_buf = malloc(OUTPUT_BUF_SIZE);

  memset(output_file_buf, '\0', OUTPUT_BUF_SIZE);

  init_input_loop(mainwin, &cursor, char_list_head);

  refresh();
  delwin(mainwin);
  endwin();

  struct CharNode *terminator = malloc(sizeof(struct CharNode));
  terminator->val = '\0';
  terminator->next = NULL;

  file_output(char_list_head, output_file_buf);
  free(output_file_buf);
  return 0;
}

void run_screen_init() {
  initscr();
  noecho();
  cbreak();
  intrflush(stdscr, FALSE);
  keypad(stdscr, TRUE);
}

void init_input_loop(WINDOW *mainwin, struct Cursor *cursor,
                     struct CharNode *char_list_head) {
  int curr_char;

  while ((curr_char = wgetch(mainwin))) {
    wrefresh(mainwin);
    bool exit_flag = false;

    switch (curr_char) {
    case 27: // ESC key
      exit_flag = true;
      break;
    case 127: // BACKSPACE
      handle_backspace_key(mainwin, cursor, char_list_head);
      continue;
    case 10: // ENTER
      handle_enter_key(mainwin, cursor, char_list_head);
      continue;
    default:
      handle_character_key(mainwin, cursor, curr_char, char_list_head);
      break;
    }

    //    printf("x value: %d, y value: %d\n", cursor->x, cursor->y);
    wrefresh(mainwin);
    if (exit_flag) {
      break;
    }
  }
}

void handle_backspace_key(WINDOW *mainwin, struct Cursor *cursor,
                          struct CharNode *char_list_head) {
  // need to multiply y position by max row length to add to x for accurate
  // position
  update_cursor_position(cursor, char_list_head, cursor->x - 1, cursor->y);
  remove_node_at_position(char_list_head,
                          cursor->x - 1 + (cursor->y * MAX_WIN_WIDTH));
  total_char_count--;
  if (total_char_count < 0) {
    total_char_count = 0;
  }
  wmove(mainwin, cursor->y -1, 0);
  wdelch(mainwin);
  //  if (total_char_count == 0) {
  //    wmove(mainwin, cursor->y - 1, cursor->x - 1);
  //    wdelch(mainwin);
  //    wmove(mainwin, cursor->y, cursor->x);
  //  }
}

void handle_enter_key(WINDOW *mainwin, struct Cursor *cursor,
                      struct CharNode *char_list_head) {
  waddch(mainwin, '\n');
  if (char_list_head->next == NULL && char_list_head->val == '\0') {
    char_list_head->val = '\n';
  } else {
    struct CharNode *new_line = malloc(sizeof(struct CharNode));
    new_line->pos = cursor->x + 1 + (cursor->y * MAX_WIN_WIDTH);
    new_line->row = cursor->y;
    new_line->prev_char_col = cursor->x - 1;
    new_line->col = cursor->x;
    new_line->val = '\n';
    new_line->next = NULL;
    struct CharNode *temp_head = char_list_head;
    while (char_list_head->next != NULL) {
      char_list_head = char_list_head->next;
    }

    char_list_head->next = new_line;
    char_list_head = temp_head;
  }
  update_cursor_position(cursor, char_list_head, 0, cursor->y + 1);
  total_char_count++;
  wmove(mainwin, cursor->y, cursor->x);
}
void handle_character_key(WINDOW *mainwin, struct Cursor *cursor, int curr_char,
                          struct CharNode *char_list_head) {
  waddch(mainwin, curr_char);
  if (char_list_head->next == NULL && char_list_head->val == '\0') {
    char_list_head->val = curr_char;
  } else {
    struct CharNode *temp_head = char_list_head;
    struct CharNode *new_char = malloc(sizeof(struct CharNode));
    new_char->val = curr_char;
    new_char->pos = cursor->x + (cursor->y * MAX_WIN_WIDTH);
    new_char->row = cursor->y;
    new_char->col = cursor->x;
    new_char->next = NULL;
    while (char_list_head->next != NULL) {
      char_list_head = char_list_head->next;
    }
    char_list_head->next = new_char;
    char_list_head = temp_head;
  }
  cursor->x++;
  total_char_count++;
  wmove(mainwin, cursor->y, cursor->x);
}
void file_output(struct CharNode *char_list_head, char *output_file_buf) {
  int real_data_count = 0;

  int file =
      open("/tmp/test_editor", O_CREAT | O_RDWR, S_IRWXU | S_IRWXO | S_IRWXG);

  char *temp_output_file_buf = malloc(total_char_count);
  int buf_counter = 0;
  while (char_list_head->next != NULL) {
    if (buf_counter > 0) {
      temp_output_file_buf[buf_counter - 1] = char_list_head->val;
    }
    buf_counter++;
    char_list_head = char_list_head->next;
  }
  // write last character to buffer manually
  temp_output_file_buf[buf_counter - 1] = char_list_head->val;
  ssize_t write_result = write(file, temp_output_file_buf, total_char_count);

  printf("real_data: %d\n", total_char_count);
}
