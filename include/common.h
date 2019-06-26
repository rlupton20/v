#pragma once

#include <stdlib.h>


#define min(a, b) (a) < (b) ? (a) : (b)
#define max(a, b) (a) < (b) ? (b) : (a)
#define KEY_ESCAPE 27


typedef enum error_t {
                      SUCCESS = 0,
                      ALLOC_ERROR = 1,
                      READ_ERROR = 2,
                      WRITE_ERROR = 3
} error_t;


typedef struct render_params_t {
  size_t height;
  size_t width;
  size_t point_locator;
} render_params_t;


typedef int event_t;
typedef struct editor_state_t editor_state_t;


/* TODO Move */
/* Mode handlers */
error_t insert_mode_handler(event_t event, editor_state_t *const state, render_params_t *const render_params);
error_t normal_mode_handler(event_t event, editor_state_t *const state, render_params_t *const render_params);
error_t command_mode_handler(event_t event, editor_state_t *const state, render_params_t *const render_params);
