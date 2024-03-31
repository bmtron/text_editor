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

void run_screen_init();
void init_input_loop(WINDOW *mainwin, struct Cursor *cursor,
                     struct StringMap *file_data);
void handle_backspace_key(WINDOW *mainwin, struct Cursor *cursor,
                          struct StringMap *file_data);
void handle_enter_key(WINDOW *mainwin, struct Cursor *cursor,
                      struct StringMap *file_data);
void handle_character_key(WINDOW *mainwin, struct Cursor *cursor, int curr_char,
                          struct StringMap *file_data);

int main(int argc, char **argv) {
  int win_len = 24;
  int win_width = 80;
  size_t map_buf_size = sizeof(struct StringMap) * win_len * win_width;
  size_t output_buf_size = sizeof(char) * win_len * win_width;

  setlocale(LC_ALL, "");
  run_screen_init();

  WINDOW *mainwin = newwin(win_len, win_width, 0, 0);
  wrefresh(mainwin);

  struct Cursor cursor = {.x = mainwin->_curx, .y = mainwin->_cury};
  struct StringMap *file_data = calloc(sizeof(struct StringMap), map_buf_size);
  char *output_file_buf = malloc(output_buf_size);

  memset(output_file_buf, '\0', output_buf_size);

  init_input_loop(mainwin, &cursor, file_data);
  refresh();
  delwin(mainwin);
  endwin();

  size_t stringmapsize = sizeof(struct StringMap);
  printf("%lu\n", stringmapsize);

  int real_data_count = 0;
  for (int i = 0; i < map_buf_size / sizeof(struct StringMap); i++) {
    if (file_data[i].data) {
      real_data_count++;
      output_file_buf[i] = file_data[i].data;
    }
  }
  int file = open("/tmp/test_editor", O_CREAT | O_RDWR,
                  S_IRWXU | S_IRWXO | S_IRWXG);

  output_file_buf = realloc(output_file_buf, sizeof(char) * real_data_count);
  ssize_t write_result =
      write(file, output_file_buf, sizeof(char) * real_data_count);

  printf("real_data: %d\n", real_data_count);

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

    wrefresh(mainwin);
    if (exit_flag) {
      break;
    }
  }
}

void handle_backspace_key(WINDOW *mainwin, struct Cursor *cursor,
                          struct StringMap *file_data) {
  waddch(mainwin, ' ');
  update_cursor_position(cursor, cursor->x - 1, cursor->y);
  wmove(mainwin, cursor->y, cursor->x);
}
void handle_enter_key(WINDOW *mainwin, struct Cursor *cursor,
                      struct StringMap *file_data) {
  waddch(mainwin, '\n');
  update_cursor_position(cursor, 0, cursor->y + 1);
  wmove(mainwin, cursor->y, cursor->x);
}
void handle_character_key(WINDOW *mainwin, struct Cursor *cursor, int curr_char,
                          struct StringMap *file_data) {
  waddch(mainwin, curr_char);
  struct StringMap new_char = {
      .col = cursor->x, .row = cursor->y, .data = curr_char};
  file_data[cursor->x + (cursor->y * cursor->x)] = new_char;
  cursor->x++;
  wmove(mainwin, cursor->y, cursor->x);
}
