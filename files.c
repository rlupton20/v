#include <stdio.h>
#include <string.h>

#include "files.h"


error_t read_file_into_editor(buffer_iter_t* const iter, const char* const filename)
{
  FILE *fp = NULL;
  char c;
  error_t ret = SUCCESS;

  fp = fopen(filename, "r");
  if (!fp) {
    return READ_ERROR;
  }

  while ((c = fgetc(fp)) != EOF && ret == SUCCESS) {
    switch (c) {
    case '\n':
      ret = append_line_at_point(iter);
      move_iter_down_line(iter);
      move_to_beginning_of_line(iter);
      break;
    default:
      ret = insert_character_at_point(iter, c);
      break;
    }
  }

  fclose(fp);

  while (!is_first_line(iter)) {
    move_iter_up_line(iter);
  }

  return SUCCESS;
}


error_t write_buffer_to_disk(buffer_iter_t *const iter, const char *const filename)
{
  FILE *fp = NULL;
  error_t ret = SUCCESS;

  size_t len = strlen(filename);
  char *swap_file = calloc(sizeof(char), len + 4);

  if (!swap_file) {
    return ALLOC_ERROR;
  }

  memcpy(swap_file, filename, len);
  strncat(swap_file, ".swp", len + 4);

  fp = fopen(swap_file, "w");

  if (fp == NULL) {
    free(swap_file);
    return WRITE_ERROR;
  }

  buffer_iter_t *write_iter = NULL;
  ret = copy_buffer_iter(iter, &write_iter);

  if (ret != SUCCESS) {
    free(swap_file);
    fclose(fp);
    return ALLOC_ERROR;
  }

  while (!is_first_line(write_iter)) {
    move_iter_up_line(write_iter);
  }

  while (true) {
    fprintf(fp, "%s\n", current_line(write_iter));
    if (is_last_line(write_iter)) {
      break;
    }
    move_iter_down_line(write_iter);
  }

  destroy_buffer_iter(write_iter);
  fclose(fp);

  rename(swap_file, filename);

  free(swap_file);

  return ret;
}
