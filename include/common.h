#pragma once

#include <stdlib.h>

#define min(a, b) (a) < (b) ? (a) : (b)
#define max(a, b) (a) < (b) ? (b) : (a)
#define KEY_ESCAPE 27

typedef enum error_t
{
  SUCCESS = 0,
  ALLOC_ERROR = 1,
  READ_ERROR = 2,
  WRITE_ERROR = 3
} error_t;

typedef struct render_params_t
{
  size_t height;
  size_t width;
  size_t point_locator;
} render_params_t;

typedef int event_t;
typedef struct editor_state_t editor_state_t;
