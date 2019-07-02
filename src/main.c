#include <ncurses.h>

#include <buffer.h>
#include <common.h>
#include <files.h>
#include <mode.h>
#include <render.h>
#include <state.h>

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
