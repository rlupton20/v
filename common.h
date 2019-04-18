#pragma once


#define min(a, b) (a) < (b) ? (a) : (b)
#define KEY_ESCAPE 27


typedef enum error_t {
                      SUCCESS = 0,
                      ALLOC_ERROR = 1
} error_t;


typedef int event_t;
typedef struct editor_state_t editor_state_t;


/* TODO Move */
/* Mode handlers */
error_t insert_mode_handler(event_t event, struct editor_state_t *const state);
error_t normal_mode_handler(event_t event, struct editor_state_t *const state);
