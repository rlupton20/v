#include <state.h>

editor_state_t*
new_editor_state(const char* const filename)
{
  editor_state_t* state = NULL;
  buffer_iter_t* buffer = new_buffer();

  if (buffer) {
    state = calloc(sizeof(editor_state_t), 1);
    if (state) {
      state->point = buffer;
      state->terminate = false;
      state->command_buffer = new_buffer();
      switch_mode(state, NORMAL);
    }
  }

  if (!state) {
    destroy_buffer(buffer);
    state = NULL;
  }

  if (state && !state->command_buffer) {
    destroy_buffer(buffer);
    free(state);
    state = NULL;
  }

  state->filename = filename;

  return state;
}

void
destroy_editor_state(editor_state_t* state)
{
  state->point = NULL;
  destroy_buffer(state->point);
  destroy_buffer(state->command_buffer);
  free(state);
}

void
switch_mode(editor_state_t* const state, editor_mode_t mode)
{
  state->mode = get_mode_handle(mode);
}

bool
should_quit(const editor_state_t* const state)
{
  return state->terminate;
}

error_t
open_line(editor_state_t* const state)
{
  error_t ret = append_line_at_point(state->point);

  if (ret == SUCCESS) {
    move_cursor_down(state);
    move_to_beginning_of_line(state->point);
    switch_mode(state, INSERT);
  }

  return ret;
}

void
move_cursor_up(editor_state_t* const state)
{
  if (!is_first_line(state->point)) {
    move_iter_up_line(state->point);
  }
}

void
move_cursor_down(editor_state_t* const state)
{
  if (!is_last_line(state->point)) {
    move_iter_down_line(state->point);
  }
}
