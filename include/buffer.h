#pragma once
/*****************************************************************************
 * buffer.h
 *
 * buffer_iter_t is a handle to a position in a buffer. It is also the
 * handle to the entire buffer.
 *
 ****************************************************************************/

#include <stdbool.h>
#include "common.h"


typedef struct buffer_iter_t buffer_iter_t;


/*
 * Create and destroy buffers
 */
buffer_iter_t* new_buffer();
void destroy_buffer(buffer_iter_t *buffer);

/*
 * Copy buffer iterators.
 * Note that edits can easily invalidate buffer iterators, so copies
 * of iterators should only be used temporarily, and for read only
 * access
 */
error_t copy_buffer_iter(const buffer_iter_t *const src, buffer_iter_t **dst);
void destroy_buffer_iter(buffer_iter_t *buffer_iter);


/*
 * Get information about the buffer iterator.
 */
char* current_line(const buffer_iter_t *const iter);
size_t column(const buffer_iter_t *const iter);
size_t line_number(const buffer_iter_t *const iter);
size_t chars_in_line(const buffer_iter_t *const iter);
bool is_last_line(const buffer_iter_t *const iter);
bool is_first_line(const buffer_iter_t *const iter);


/*
 * Move around the buffer
 */
void move_iter_down_line(buffer_iter_t *const iter);
void move_iter_up_line(buffer_iter_t *const iter);
void move_iter_forward_char(buffer_iter_t *const iter);
void move_iter_back_char(buffer_iter_t *const iter);
void move_to_beginning_of_line(buffer_iter_t *const iter);


/*
 * Modify the buffer at the buffer iterator
 */
error_t append_line_at_point(buffer_iter_t *const iter);
error_t insert_character_at_point(buffer_iter_t *const iter, const char c);
void delete_character_at_point(buffer_iter_t *const iter);
void clear_line_at_point(buffer_iter_t *const iter);
