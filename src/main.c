#include <ncurses.h>

#include <buffer.h>
#include <common.h>
#include <files.h>
#include <mode.h>
#include <state.h>
#include <render.h>

/*
 * Takes an event and updates the editor state accordingly.
 */
error_t
update(const event_t event, editor_state_t* const state);

int
main(int argc, char* argv[])
{
  const char* const filename = argc > 1 ? argv[1] : NULL;

  initscr();
  noecho();

  editor_state_t* state = new_editor_state(filename);

  if (!state) {
    endwin();
    return 1;
  }

  if (filename && read_file_into_editor(state->point, filename) != SUCCESS) {
    endwin();
    return 1;
  }

  render_params_t render_params = { 0 };

  do {
    update_render_params(&render_params);
    render(state, &render_params);

    const event_t event = getch();

    if (update(event, state) != SUCCESS) {
      endwin();
      exit(1);
    }
  } while (!should_quit(state));

  endwin();
  destroy_editor_state(state);

  return 0;
}

error_t
update(const event_t event, editor_state_t* const state)
{
  return (state->mode->handler)(event, state);
}

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
