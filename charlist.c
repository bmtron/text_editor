#include "charlist.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

struct CharNode *get_last_character_in_row(int row,
                                           struct CharNode *char_list_head) {
  struct CharNode *tmp_head_ptr = malloc(sizeof(struct CharNode));
  tmp_head_ptr = char_list_head;

  while (char_list_head->row < row && char_list_head->next != NULL)  {
    char_list_head = char_list_head->next;
  }


  int file =
      open("/tmp/editor_dump", O_CREAT | O_RDWR, S_IRWXU | S_IRWXO | S_IRWXG);
  char *buf = malloc(sizeof(char));
  buf = &char_list_head->val;
  write(file, buf, sizeof(char));
  struct CharNode *return_ptr = malloc(sizeof(struct CharNode));
  return_ptr = char_list_head;

  char_list_head = tmp_head_ptr;

  return return_ptr;
}
void remove_node_at_position(struct CharNode *char_list_head, int position) {
  if (position == 0 && char_list_head->next == NULL) {
    char_list_head->val = '\0';
    return;
  }
  int node_count = 0;
  struct CharNode *temp_list_head = char_list_head;
  struct CharNode *prev_node = char_list_head;
  while (node_count <= position && char_list_head->next != NULL) {
    prev_node = char_list_head;
    char_list_head = char_list_head->next;
    node_count++;
  }

  struct CharNode *temp_nullable_head = char_list_head;
  prev_node->next = char_list_head->next;
  free(temp_nullable_head);

  char_list_head = temp_list_head;
}
