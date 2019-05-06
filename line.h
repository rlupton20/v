#pragma once

/*****************************************************************************
 * line.h
 *
 * line_t provides an automatically growing data structure
 * for storing characters.
 *
 ****************************************************************************/


#include "common.h"


typedef struct line_t line_t;


error_t allocate_line(line_t *const line);
void deallocate_line(line_t *const line);


error_t insert_character(line_t *const line, const char c, const size_t ix);
void delete_character(line_t *const line, size_t ix);
