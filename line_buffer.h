#pragma once

#include "common.h"


#define DEFAULT_LINE_BUFFER_LENGTH 120


typedef struct line_buffer_t {
  size_t length;
  char *buffer;
} line_buffer_t;


line_buffer_t* new_line_buffer(size_t capacity);

void destroy_line_buffer(line_buffer_t *line_buffer);

error_t grow_buffer(line_buffer_t *const line_buffer);


error_t insert_character(line_buffer_t *const line_buffer,
                         const char c,
                         const size_t ix);
