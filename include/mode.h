#pragma once

#include <common.h>

typedef enum editor_mode_t
{
  NORMAL,
  INSERT,
  COMMAND
} editor_mode_t;

typedef error_t(mode_handler_t)(event_t event,
                                struct editor_state_t* const state,
                                render_params_t* const render_params);

typedef struct mode_t
{
  const char* const name;
  mode_handler_t* const handler;
} mode_t;

const mode_t*
get_mode_handle(editor_mode_t mode);

error_t
insert_mode_handler(event_t event,
                    editor_state_t* const state,
                    render_params_t* const render_params);
error_t
normal_mode_handler(event_t event,
                    editor_state_t* const state,
                    render_params_t* const render_params);
error_t
command_mode_handler(event_t event,
                     editor_state_t* const state,
                     render_params_t* const render_params);
