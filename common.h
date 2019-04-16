#pragma once


#define min(a, b) (a) < (b) ? (a) : (b)
#define KEY_ESCAPE 27


typedef enum error_t {
                      SUCCESS = 0,
                      ALLOC_ERROR = 1
} error_t;


typedef int event_t;
typedef struct editor_state_t editor_state_t;
typedef error_t (mode_handler_t)(event_t event,
                                 struct editor_state_t *const state);

/* Mode handlers */
error_t insert_mode_handler(event_t event, struct editor_state_t *const state);
error_t normal_mode_handler(event_t event, struct editor_state_t *const state);


typedef struct mode_t {
  const char *const name;
  mode_handler_t *const handler;
} mode_t;
