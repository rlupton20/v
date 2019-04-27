#pragma once

#include <stdbool.h>
#include "common.h"


typedef struct buffer_iter_t buffer_iter_t;

buffer_iter_t* new_buffer();
void destroy_buffer(buffer_iter_t *buffer);
void destroy_buffer_iter(buffer_iter_t *buffer_iter);
error_t copy_buffer_iter(const buffer_iter_t *const src, buffer_iter_t **dst);

/* Getters */
char* current_line(const buffer_iter_t *const iter);
size_t column(const buffer_iter_t *const iter);
size_t line_number(const buffer_iter_t *const iter);
size_t chars_in_line(const buffer_iter_t *const iter);
bool is_last_line(const buffer_iter_t *const iter);
bool is_first_line(const buffer_iter_t *const iter);

/* Movement */
void move_iter_down_line(buffer_iter_t *const iter);
void move_iter_up_line(buffer_iter_t *const iter);
void move_iter_forward_char(buffer_iter_t *const iter);
void move_iter_back_char(buffer_iter_t *const iter);
void move_iter_to_top(buffer_iter_t *const iter);
void move_to_beginning_of_line(buffer_iter_t *const iter);

/* Modification */
error_t append_line_at_point(buffer_iter_t *const iter);
error_t insert_character_at_point(buffer_iter_t *const iter, const char c);
void delete_character_at_point(buffer_iter_t *const iter);
