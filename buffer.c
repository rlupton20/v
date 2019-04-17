#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"


// The most a line buffer can expand by
const size_t default_line_buffer_length = 120;
const size_t max_buffer_growth = 240;


typedef struct buffer_cell_t buffer_cell_t;
typedef buffer_cell_t* xorptr_t;


typedef struct line_t {
  size_t length;
  char *buffer;
} line_t;


struct buffer_cell_t {
  line_t line;
  xorptr_t neighbours;
};


struct buffer_iter_t {
  buffer_cell_t *current;
  buffer_cell_t *next;
  buffer_cell_t *previous;
  size_t column;
  size_t line;
};


buffer_cell_t* new_buffer_cell();
void destroy_buffer_cell(buffer_cell_t *const buffer_cell);


error_t allocate_line(line_t *const line);
void deallocate_line(line_t *const line);
error_t grow_buffer(line_t *const line);
error_t insert_character(line_t *const line, const char c, const size_t ix);


xorptr_t encode_pair(const buffer_cell_t *const a, const buffer_cell_t *b);
buffer_cell_t* decode_with(const xorptr_t encoded, const buffer_cell_t *v);


buffer_iter_t* new_buffer()
{
  buffer_iter_t* buffer = NULL;
  buffer_cell_t* buffer_cell = new_buffer_cell();

  if (buffer_cell) {
    buffer = calloc(sizeof(buffer_iter_t), 1);
    if (buffer) {
      buffer->current = buffer_cell;
      buffer->previous = NULL;
      buffer->next = NULL;
      buffer->column = 0;
    } else {
      destroy_buffer_cell(buffer_cell);
    }
  }

  return buffer;
}


void destroy_buffer(buffer_iter_t *buffer)
{
  // TODO - do a bidirectional free
  return;
}


error_t append_line_at_point(buffer_iter_t *const iter) {
  buffer_cell_t *new_cell = new_buffer_cell();

  if (new_cell) {
    new_cell->neighbours = encode_pair(iter->current, iter->next);

    if (iter->next) {
      const buffer_cell_t* nexts_next =
        decode_with(iter->next->neighbours, iter->current);
      iter->next->neighbours = encode_pair(new_cell, nexts_next);
    } else {
      iter->next = new_cell;
    }

  }

  return new_cell ? SUCCESS : ALLOC_ERROR;
}


error_t insert_character_at_point(buffer_iter_t *const iter, char c)
{
  return insert_character(&iter->current->line, c, iter->column++);
}


char* current_line(const buffer_iter_t *const iter)
{
  return iter->current->line.buffer;
}


size_t column(const buffer_iter_t *const iter)
{
  return iter->column;
}


/* Functions for handling buffer cells */

buffer_cell_t* new_buffer_cell()
{
  buffer_cell_t *buffer_cell = calloc(sizeof(buffer_cell_t), 1);

  if (buffer_cell) {
    const error_t alloc_ret = allocate_line(&buffer_cell->line);
    if (alloc_ret == SUCCESS) {
      buffer_cell->neighbours = NULL;
    } else {
      free(buffer_cell);
      buffer_cell = NULL;
    }
  }

  return buffer_cell;
}


void destroy_buffer_cell(buffer_cell_t *const buffer_cell)
{
  deallocate_line(&buffer_cell->line);
  free(buffer_cell);
}


xorptr_t encode_pair(const buffer_cell_t *const a, const buffer_cell_t *b)
{
  return (xorptr_t)((uintptr_t)a ^ (uintptr_t)b);
}


buffer_cell_t* decode_with(const xorptr_t encoded, const buffer_cell_t *v)
{
  return (buffer_cell_t *)((uintptr_t)encoded ^ (uintptr_t)v);
}


/* Functions for handling lines */

error_t allocate_line(line_t *const line)
{
  line->buffer = calloc(sizeof(char), default_line_buffer_length + 1);

  if (line->buffer) {
    line->length = default_line_buffer_length;
  }

  return line->buffer ? SUCCESS : ALLOC_ERROR;
}


void deallocate_line(line_t *const line)
{
  free(line->buffer);
  line->buffer = NULL;
  line->length = 0;
}


error_t grow_buffer(line_t *const line)
{
  const size_t new_size =
    min(max_buffer_growth + line->length, 2 * line->length);

  char* const new_buffer = realloc(line->buffer, new_size + 1);
  if (new_buffer) {
    memset(new_buffer + line->length, 0, new_size - line->length + 1);
    line->buffer = new_buffer;
    line->length = new_size;
  }

  return new_buffer ? SUCCESS : ALLOC_ERROR;
}


error_t insert_character(line_t *const line, const char c, const size_t ix)
{
  if (ix < line->length) {
    line->buffer[ix] = c;
    return SUCCESS;
  }

  const error_t grow_ret = grow_buffer(line);
  return grow_ret == SUCCESS ?
    insert_character(line, c, ix) : grow_ret;
}


void move_point_down_line(buffer_iter_t *const iter)
{
  if (iter->next) {
    buffer_cell_t *next_next = decode_with(iter->next->neighbours,
                                           iter->current);
    iter->previous = iter->current;
    iter->current = iter->next;
    iter->next = next_next;
  }
}
