#pragma once

#include "line_buffer.h"


typedef struct buffer_iter_t buffer_iter_t;

buffer_iter_t* new_buffer();
// TODO Need a way to clean up buffers

error_t append_line_at_point(buffer_iter_t *const iter);

error_t insert_character_at_point(buffer_iter_t *const iter, const char c);

