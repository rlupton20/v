#include <stdint.h>
#include <stdlib.h>

#include "buffer.h"


typedef struct buffer_cell_t buffer_cell_t;
typedef buffer_cell_t* xorptr_t;


// Can this be joined with line_buffer ?
struct buffer_cell_t {
  line_buffer_t *line;
  xorptr_t neighbours;
};


struct buffer_iter_t {
  buffer_cell_t *current;
  buffer_cell_t *next;
  buffer_cell_t *previous;
  size_t column;
};


buffer_cell_t* new_buffer_cell();
void destroy_buffer_cell(buffer_cell_t *const buffer_cell);

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


error_t append_line_at_point(buffer_iter_t *const iter) {
  buffer_cell_t *new_cell = new_buffer_cell();

  if (new_cell) {
    new_cell->neighbours = encode_pair(iter->current, iter->next);

    if (iter->next) {
      const buffer_cell_t* nexts_next =
        decode_with(iter->next->neighbours, iter->current);
      iter->next->neighbours = encode_pair(new_cell, nexts_next);
    }

  }

  return new_cell ? SUCCESS : ALLOC_ERROR;
}


error_t insert_character_at_point(buffer_iter_t *const iter, char c)
{
  return insert_character(iter->current->line, c, iter->column++);
}


buffer_cell_t* new_buffer_cell()
{
  buffer_cell_t *buffer_cell = NULL;
  line_buffer_t *const line = new_line_buffer(DEFAULT_LINE_BUFFER_LENGTH);

  if (line) {
    buffer_cell = calloc(sizeof(buffer_cell_t), 1);
    if (buffer_cell) {
      buffer_cell->line = line;
      buffer_cell->neighbours = NULL;
    } else {
      free(line);
    }
  }

  return buffer_cell;
}


void destroy_buffer_cell(buffer_cell_t *const buffer_cell)
{
  destroy_line_buffer(buffer_cell->line);
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
