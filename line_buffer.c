#include <string.h>
#include <stdlib.h>

#include "line_buffer.h"


// The most a line buffer can expand by
const size_t max_buffer_growth = 240;


line_buffer_t* new_line_buffer(const size_t capacity)
{
  line_buffer_t *line_buffer = NULL;
  char *const buffer = calloc(sizeof(char), capacity + 1);
  if (buffer) {
    line_buffer = calloc(sizeof(line_buffer_t), 1);
    if (line_buffer) {
      line_buffer->length = capacity;
      line_buffer->buffer = buffer;
    } else {
      free(buffer);
    }
  }
  return line_buffer;
}


void destroy_line_buffer(line_buffer_t *line_buffer) {
  free(line_buffer->buffer);
  free(line_buffer);
}


error_t grow_buffer(line_buffer_t *const line_buffer) {
  const size_t new_size =
    min(max_buffer_growth + line_buffer->length, 2 * line_buffer->length) + 1;

  char* const new_buffer = realloc(line_buffer->buffer, new_size);
  if (new_buffer) {
    memset(new_buffer + line_buffer->length, 0, new_size - line_buffer->length);
    line_buffer->buffer = new_buffer;
    line_buffer->length = new_size - 1;
    return SUCCESS;
  } else {
    return ALLOC_ERROR;
  }
  return SUCCESS;
}


error_t insert_character(line_buffer_t *const line_buffer,
                      const char c,
                      const size_t ix)
{
  if (ix < line_buffer->length) {
    (line_buffer->buffer)[ix] = c;
    return SUCCESS;
  } else {
    const error_t grow = grow_buffer(line_buffer);
    return grow == SUCCESS ?
      insert_character(line_buffer, c, ix) : grow;
  }
  return SUCCESS;
}
