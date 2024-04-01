#include "struct_defs.h"
#include "bcursor.h"
#include "stringmap.h"

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
                     struct StringMap *file_data);
void handle_backspace_key(WINDOW *mainwin, struct Cursor *cursor,
                          struct StringMap *file_data);
void handle_enter_key(WINDOW *mainwin, struct Cursor *cursor,
                      struct StringMap *file_data);
void handle_character_key(WINDOW *mainwin, struct Cursor *cursor, int curr_char,
                          struct StringMap *file_data);
void file_output(struct StringMap *file_data, char *output_file_buf);

int main(int argc, char **argv) {

  setlocale(LC_ALL, "");
  run_screen_init();

  WINDOW *mainwin = newwin(MAX_WIN_HEIGHT, MAX_WIN_WIDTH, 0, 0);
  wrefresh(mainwin);

  struct Cursor cursor = {.x = mainwin->_curx, .y = mainwin->_cury};
  struct StringMap *file_data =
      calloc(sizeof(struct StringMap), STRMAP_BUF_SIZE);
  char *output_file_buf = malloc(OUTPUT_BUF_SIZE);

  memset(output_file_buf, '\0', OUTPUT_BUF_SIZE);

  init_input_loop(mainwin, &cursor, file_data);
  refresh();
  delwin(mainwin);
  endwin();

  file_output(file_data, output_file_buf);
  free(file_data);
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
                     struct StringMap *file_data) {
  int curr_char;

  while ((curr_char = wgetch(mainwin))) {
    wrefresh(mainwin);
    bool exit_flag = false;

    switch (curr_char) {
    case 27: // ESC key
      exit_flag = true;
      break;
    case 127: // BACKSPACE
      handle_backspace_key(mainwin, cursor, file_data);
      continue;
    case 10: // ENTER
      handle_enter_key(mainwin, cursor, file_data);
      continue;
    default:
      handle_character_key(mainwin, cursor, curr_char, file_data);
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
                          struct StringMap *file_data) {
  printf("TESTBACKSPACE");
  waddch(mainwin, ' ');
  struct StringMap *strmap = get_character_at_cursor(cursor, file_data);
  strmap = NULL;
  update_cursor_position(cursor, file_data, cursor->x - 1, cursor->y);
  wmove(mainwin, cursor->y, cursor->x);
}
void handle_enter_key(WINDOW *mainwin, struct Cursor *cursor,
                      struct StringMap *file_data) {
  waddch(mainwin, '\n');
  struct StringMap new_line = {
      .col = cursor->x + 1, .row = cursor->y, .data = '\n'};
  file_data[cursor->x + 1 + (cursor->y * MAX_WIN_WIDTH)] = new_line;
  printf("cursor x: %d cursor y: %d\n", cursor->x, cursor->y);
  update_cursor_position(cursor, file_data, 0, cursor->y + 1);
  wmove(mainwin, cursor->y, cursor->x);
}
void handle_character_key(WINDOW *mainwin, struct Cursor *cursor, int curr_char,
                          struct StringMap *file_data) {
  waddch(mainwin, curr_char);
  struct StringMap new_char = {
      .col = cursor->x, .row = cursor->y, .data = curr_char};
  file_data[cursor->x + (cursor->y * MAX_WIN_WIDTH)] = new_char;
  cursor->x++;
  wmove(mainwin, cursor->y, cursor->x);
}
void file_output(struct StringMap *file_data, char *output_file_buf) {

  size_t stringmapsize = sizeof(struct StringMap);
  printf("%lu\n", stringmapsize);

  int real_data_count = 0;
  struct LList *original_head = malloc(sizeof(struct LList));
  original_head->val = '\0';
  original_head->next = NULL;

  for (int i = 0; i < OUTPUT_BUF_SIZE; i++) {
    if (file_data[i].data) {
      struct LList *head = original_head;

      real_data_count++;
      while (head->next != NULL) {
        head = head->next;
      }
      struct LList *next = malloc(sizeof(struct LList));
      next->val = '\0';
      next->next = NULL;
      head->val = file_data[i].data;
      head->next = next;
      output_file_buf[i] = file_data[i].data;
      if (file_data[i].data == '\n') {
        printf("NEW LINE ENCOUNTERED\n");
      } else {
        printf("%c\n", file_data[i].data);
        printf("new buffer data: %c\n", output_file_buf[i]);
      }
    }
  }
  int file =
      open("/tmp/test_editor", O_CREAT | O_RDWR, S_IRWXU | S_IRWXO | S_IRWXG);

  char *temp_output_file_buf = malloc(real_data_count);
  int buf_counter = 0;
  while (original_head->next != NULL) {
      temp_output_file_buf[buf_counter] = original_head->val;
      buf_counter++;
      original_head = original_head->next;
  }
  
  ssize_t write_result = write(file, temp_output_file_buf, real_data_count);

  printf("real_data: %d\n", real_data_count);
}
