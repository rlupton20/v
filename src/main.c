#include <ncurses.h>
#include <string.h>

#include <buffer.h>
#include <common.h>
#include <files.h>
#include <mode.h>
#include <state.h>

/*
 * Takes an event and updates the editor state accordingly.
 */
error_t
update(const event_t event,
       editor_state_t* const state,
       render_params_t* const render_params);

/*
 * Render the current editor state to sceen.
 */
void
render(const editor_state_t* const state, const render_params_t* const params);

/*
 * Render the modeline.
 */
void
render_modeline(const editor_state_t* const state,
                const render_params_t* const render_params);

/*
 * Render the command buffer.
 */
void
render_command_buffer(const editor_state_t* const state,
                      const render_params_t* const render_params);

/*
 * Update the render parameters.
 */
void
update_render_params(render_params_t* const render_params);

/*
 * Open a new line, and enter insert mode.
 */
error_t
open_line(editor_state_t* const state, render_params_t* const render_params);

/*
 * terminal_lines determines the number of lines on the screen the line
 * at iter will use.
 */
size_t
terminal_lines(size_t terminal_width, const buffer_iter_t* const iter);

/*
 * locate_start_of_render locates the line where the editor should
 * start drawing to screen.
 */
size_t
locate_start_of_render(const render_params_t* const params,
                       buffer_iter_t* render_point);

/*
 * Move the cursor up a line.
 */
void
move_cursor_up(editor_state_t* const state,
               render_params_t* const render_params);

/*
 * Move the cursor down a line.
 */
void
move_cursor_down(editor_state_t* const state,
                 render_params_t* const render_params);

/*
 * Execute the command in the command buffer.
 */
void
execute_command(editor_state_t* const state);

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

    if (update(event, state, &render_params) != SUCCESS) {
      endwin();
      exit(1);
    }
  } while (!should_quit(state));

  endwin();
  destroy_editor_state(state);

  return 0;
}

error_t
update(const event_t event,
       editor_state_t* const state,
       render_params_t* const render_params)
{
  return (state->mode->handler)(event, state, render_params);
}

void
update_render_params(render_params_t* const render_params)
{
  getmaxyx(stdscr, render_params->height, render_params->width);
}

void
render(const editor_state_t* const state,
       const render_params_t* const render_params)
{
  buffer_iter_t* render_point;
  copy_buffer_iter(state->point, &render_point);

  size_t row = 0;

  erase();
  render_modeline(state, render_params);

  size_t current = locate_start_of_render(render_params, render_point);
  while (current + 2 < render_params->height) {
    mvprintw(current, 0, "%s", current_line(render_point));

    if (current_line(render_point) == current_line(state->point)) {
      row = current;
    }

    if (is_last_line(render_point)) {
      break;
    }

    current += terminal_lines(render_params->width, render_point);
    move_iter_down_line(render_point);
  }

  destroy_buffer_iter(render_point);

  render_command_buffer(state, render_params);

  move(row, column(state->point));

  refresh();
}

void
render_modeline(const editor_state_t* const state,
                const render_params_t* const render_params)
{
  attron(A_BOLD);
  mvprintw(render_params->height - 2,
           0,
           "%d:%d\t%s",
           line_number(state->point),
           column(state->point),
           state->mode->name);
  attroff(A_BOLD);
}

void
render_command_buffer(const editor_state_t* const state,
                      const render_params_t* const render_params)
{
  mvprintw(
    render_params->height - 1, 0, "%s", current_line(state->command_buffer));
}

error_t
insert_mode_handler(event_t event,
                    struct editor_state_t* const state,
                    render_params_t* const render_params)
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
      ret = open_line(state, render_params);
      break;
    default:
      ret = insert_character_at_point(state->point, event);
      break;
  }

  return ret;
}

error_t
normal_mode_handler(event_t event,
                    struct editor_state_t* const state,
                    render_params_t* const render_params)
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
      move_cursor_down(state, render_params);
      break;
    case 'k':
      move_cursor_up(state, render_params);
      break;
    case 'l':
      move_iter_forward_char(state->point);
      break;
    case 'o':
      ret = open_line(state, render_params);
      break;
    default:
      break;
  }

  return ret;
}

error_t
command_mode_handler(event_t event,
                     struct editor_state_t* const state,
                     render_params_t* const render_params)
{
  error_t ret = SUCCESS;

  switch (event) {
    case KEY_ESCAPE:
      clear_line_at_point(state->command_buffer);
      switch_mode(state, NORMAL);
      break;
    case '\n':
      execute_command(state);
      break;
    default:
      ret = insert_character_at_point(state->command_buffer, event);
      break;
  }

  return ret;
}

error_t
open_line(editor_state_t* const state, render_params_t* const render_params)
{
  error_t ret = append_line_at_point(state->point);

  if (ret == SUCCESS) {
    move_cursor_down(state, render_params);
    move_to_beginning_of_line(state->point);
    switch_mode(state, INSERT);
  }

  return ret;
}

/* Rendering questions */

size_t
terminal_lines(size_t terminal_width, const buffer_iter_t* const iter)
{
  return chars_in_line(iter) / terminal_width + 1;
}

size_t
locate_start_of_render(const render_params_t* const params,
                       buffer_iter_t* render_point)
{
  size_t point_locator = params->point_locator;
  while (!is_first_line(render_point)) {
    move_iter_up_line(render_point);

    if (terminal_lines(params->width, render_point) > point_locator) {
      break;
    }

    point_locator -= terminal_lines(params->width, render_point);
  }

  if (!is_first_line(render_point)) {
    move_iter_down_line(render_point);
  }

  return point_locator;
}

void
move_cursor_up(editor_state_t* const state,
               render_params_t* const render_params)
{
  if (!is_first_line(state->point)) {
    move_iter_up_line(state->point);

    const size_t lines_for_current =
      terminal_lines(render_params->width, state->point);

    if (lines_for_current > render_params->point_locator) {
      render_params->point_locator = 0;
    } else {
      render_params->point_locator -= lines_for_current;
    }
  }
}

void
move_cursor_down(editor_state_t* const state,
                 render_params_t* const render_params)
{
  if (!is_last_line(state->point)) {
    const size_t lines_for_current =
      terminal_lines(render_params->width, state->point);

    render_params->point_locator =
      min(render_params->height - 3,
          render_params->point_locator + lines_for_current);

    move_iter_down_line(state->point);
  }
}

void
execute_command(editor_state_t* const state)
{
  const char* cmd = current_line(state->command_buffer);
  cmd++; // Skip initial ':'

  while (*cmd != '\0' && !should_quit(state)) {
    switch (*cmd) {

      case 'q':
        state->terminate = true;
        break;

      case 'w':
        if (state->filename) {
          write_buffer_to_disk(state->point, state->filename);
        }
        break;

      default:
        break;
    }
    cmd++;
  }

  clear_line_at_point(state->command_buffer);
  switch_mode(state, NORMAL);
}
