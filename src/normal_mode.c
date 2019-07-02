#include <mode.h>
#include <state.h>

error_t
normal_mode_handler(event_t event, struct editor_state_t* const state)
{
  error_t ret = SUCCESS;

  switch (event) {
    case 'i':
      switch_mode(state, INSERT);
      break;
    case ':':
      ret = insert_character_at_point(state->command_buffer, ':');
      switch_mode(state, COMMAND);
      break;
    case 'h':
      move_iter_back_char(state->point);
      break;
    case 'j':
      move_cursor_down(state);
      break;
    case 'k':
      move_cursor_up(state);
      break;
    case 'l':
      move_iter_forward_char(state->point);
      break;
    case 'o':
      ret = open_line(state);
      break;
    default:
      break;
  }

  return ret;
}
