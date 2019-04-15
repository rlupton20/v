#pragma once

#include "common.h"


typedef struct buffer_iter_t buffer_iter_t;

buffer_iter_t* new_buffer();

void destroy_buffer(buffer_iter_t *buffer);

error_t append_line_at_point(buffer_iter_t *const iter);

error_t insert_character_at_point(buffer_iter_t *const iter, const char c);

char* current_line(const buffer_iter_t *const iter);

size_t column(const buffer_iter_t *const iter);

