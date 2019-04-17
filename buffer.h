#pragma once

#include "common.h"


typedef struct buffer_iter_t buffer_iter_t;

buffer_iter_t* new_buffer();
void destroy_buffer(buffer_iter_t *buffer);

/* Getters */
char* current_line(const buffer_iter_t *const iter);
size_t column(const buffer_iter_t *const iter);

/* Movement */
void move_point_down_line(buffer_iter_t *const iter);

/* Modification */
error_t append_line_at_point(buffer_iter_t *const iter);
error_t insert_character_at_point(buffer_iter_t *const iter, const char c);


