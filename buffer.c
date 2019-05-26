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
  size_t used;
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


// Buffer helper function declarations
buffer_cell_t* new_buffer_cell();
void destroy_buffer_cell(buffer_cell_t *const buffer_cell);

xorptr_t encode_pair(const buffer_cell_t *const a, const buffer_cell_t *b);
buffer_cell_t* decode_with(const xorptr_t encoded, const buffer_cell_t *v);

// Line helper function declarations
error_t allocate_line(line_t *const line);
void deallocate_line(line_t *const line);
error_t insert_character(line_t *const line, const char c, const size_t ix);
void delete_character(line_t *const line, size_t ix);
error_t grow_buffer(line_t *const line);
void clear_line(line_t *const line);


/*****************************************************************************/
/* Buffer lifecycle                                                          */
/*****************************************************************************/
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


void destroy_buffer_iter(buffer_iter_t *buffer_iter)
{
  free(buffer_iter);
}


error_t copy_buffer_iter(const buffer_iter_t *const src, buffer_iter_t **dst)
{
  buffer_iter_t *copy = new_buffer();
  if (copy) {
    *dst = copy;
    *copy = *src;
  }

  return copy ? SUCCESS : ALLOC_ERROR;
}


/*****************************************************************************/
/* Get information about the buffer                                          */
/*****************************************************************************/
char* current_line(const buffer_iter_t *const iter)
{
  return iter->current->line.buffer;
}


size_t column(const buffer_iter_t *const iter)
{
  return min(iter->column, iter->current->line.used);
}


size_t line_number(const buffer_iter_t *const iter)
{
  return iter->line;
}


size_t chars_in_line(const buffer_iter_t *const iter)
{
  return iter->current->line.used;
}


bool is_last_line(const buffer_iter_t *const iter)
{
  return iter->next == NULL;
}


bool is_first_line(const buffer_iter_t *const iter)
{
  return iter->previous == NULL;
}

/*****************************************************************************/
/* Buffer movement functions                                                 */
/*****************************************************************************/
void move_iter_down_line(buffer_iter_t *const iter)
{
  if (iter->next) {
    buffer_cell_t *next_next = decode_with(iter->next->neighbours,
                                           iter->current);
    iter->previous = iter->current;
    iter->current = iter->next;
    iter->next = next_next;
    iter->line++;
  }
}


void move_iter_up_line(buffer_iter_t *const iter)
{
  if (iter->previous) {
    buffer_cell_t *prev_prev = decode_with(iter->previous->neighbours,
                                           iter->current);
    iter->next = iter->current;
    iter->current = iter->previous;
    iter->previous = prev_prev;
    iter->line--;
  }
}


void move_iter_back_char(buffer_iter_t *const iter)
{
  if (iter->column > 0) {
    iter->column--;
  }
}


void move_iter_forward_char(buffer_iter_t *const iter)
{
  iter->column = min(iter->column + 1, iter->current->line.used);
}


void move_to_beginning_of_line(buffer_iter_t *const iter)
{
  iter->column = 0;
}


/*****************************************************************************/
/* Buffer movement functions                                                 */
/*****************************************************************************/
error_t append_line_at_point(buffer_iter_t *const iter) {
  buffer_cell_t *new_cell = new_buffer_cell();

  if (new_cell) {
    new_cell->neighbours = encode_pair(iter->current, iter->next);
    iter->current->neighbours = encode_pair(iter->previous, new_cell);

    if (iter->next) {
      const buffer_cell_t* nexts_next =
        decode_with(iter->next->neighbours, iter->current);
      iter->next->neighbours = encode_pair(new_cell, nexts_next);
    }

    iter->next = new_cell;

  }

  return new_cell ? SUCCESS : ALLOC_ERROR;
}


error_t insert_character_at_point(buffer_iter_t *const iter, char c)
{
  return insert_character(&iter->current->line, c, iter->column++);
}


void delete_character_at_point(buffer_iter_t *const iter)
{
  size_t ix = column(iter);
  move_iter_back_char(iter);
  delete_character(&iter->current->line, ix);
}

void clear_line_at_point(buffer_iter_t *const iter)
{
  clear_line(&iter->current->line);
}

/*****************************************************************************/
/* Helper functions and intermediate structures                              */
/*****************************************************************************/

/* ------------------------------------------------------------------------- */
/* Buffer cell lifecycle and management                                      */
/* ------------------------------------------------------------------------- */
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


/* ------------------------------------------------------------------------- */
/* Buffer cell character operations                                          */
/* ------------------------------------------------------------------------- */
error_t insert_character(line_t *const line, const char c, size_t ix)
{
  ix = min(ix, line->used);
  if (line->used < line->length) {  // TODO Check
    // Make space for the new character
    memmove(line->buffer + ix + 1, line->buffer + ix, line->used - ix);

    line->buffer[ix] = c;
    line->used++;

    return SUCCESS;
  }

  const error_t grow_ret = grow_buffer(line);
  return grow_ret == SUCCESS ?
    insert_character(line, c, ix) : grow_ret;
}

void delete_character(line_t *const line, size_t ix)
{
  if (ix) {
    memmove(line->buffer + ix - 1,
            line->buffer + ix,
            line->used - ix);
    line->buffer[line->used - 1] = '\0';
    line->used--;
  }
}


void clear_line(line_t *const line)
{
  memset(line->buffer, 0, line->length);
  line->used = 0;
}


/* ------------------------------------------------------------------------- */
/* Buffer cell encoding functions                                            */
/* ------------------------------------------------------------------------- */
xorptr_t encode_pair(const buffer_cell_t *const a, const buffer_cell_t *b)
{
  return (xorptr_t)((uintptr_t)a ^ (uintptr_t)b);
}


buffer_cell_t* decode_with(const xorptr_t encoded, const buffer_cell_t *v)
{
  return (buffer_cell_t *)((uintptr_t)encoded ^ (uintptr_t)v);
}
