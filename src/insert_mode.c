#include <mode.h>
#include <state.h>

error_t
insert_mode_handler(event_t event, struct editor_state_t* const state)
{
  error_t ret = SUCCESS;

  switch (event) {
    case KEY_ESCAPE:
      switch_mode(state, NORMAL);
      break;
    case 127: // TODO Why is this not KEY_BACKSPACE?
      delete_character_at_point(state->point);
      break;
    case '\n':
      ret = open_line(state);
      break;
    default:
      ret = insert_character_at_point(state->point, event);
      break;
  }

  return ret;
}
