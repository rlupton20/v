#include <ncurses.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "common.h"
#include "buffer.h"
#include "mode.h"


typedef struct render_params_t {
  size_t height;
  size_t width;
  size_t point_locator;
} render_params_t;


struct editor_state_t {
  buffer_iter_t *point;
  const mode_t *mode;
  render_params_t render_params;
  bool terminate;
};


/* Functions for working with editor state */
editor_state_t* new_editor_state();
void destroy_editor_state(editor_state_t *state);


/* Functions for running the editor */
error_t update(const event_t event, editor_state_t *const state);
void render(const editor_state_t *const state);
void render_modeline(const editor_state_t *const state);

/* Editor state functions */
bool should_quit(const editor_state_t *const state);
void switch_mode(editor_state_t *const state, editor_mode_t mode);
void open_line(buffer_iter_t *const iter);

/* Functions for resolving rendering questions */
size_t terminal_lines(const render_params_t *const params,
                      const buffer_iter_t *const iter);
size_t locate_start_of_render(const render_params_t *const params,
                              buffer_iter_t *render_point);
void move_cursor_up(editor_state_t *const state);
void move_cursor_down(editor_state_t *const state);


int main(int argv, char *argc[])
{
  editor_state_t *state = new_editor_state();

  if (!state) {
    return 1;
  }

  initscr();
  noecho();
  render(state);

  while (!should_quit(state)) {
    const event_t event = getch();
    if (update(event, state) != SUCCESS) {
      exit(1);
    }
    render(state);
  }

  endwin();
  destroy_editor_state(state);

  return 0;
}


error_t update(const event_t event, editor_state_t *const state)
{
  getmaxyx(stdscr, state->render_params.height, state->render_params.width);
  return (state->mode->handler)(event, state);
}


void render(const editor_state_t *const state)
{
  buffer_iter_t *render_point;
  copy_buffer_iter(state->point, &render_point);

  size_t row = 0;

  clear();
  render_modeline(state);

  size_t current = locate_start_of_render(&state->render_params, render_point);
  while (true) {
    mvprintw(current, 0, "%s", current_line(render_point));

    if (current_line(render_point) == current_line(state->point)) {
      row = current;
    }

    if (is_last_line(render_point)) {
      break;
    }

    current += terminal_lines(&state->render_params, render_point);
    move_iter_down_line(render_point);
  }

  destroy_buffer_iter(render_point);

  move(row, column(state->point));

  refresh();
}


void render_modeline(const editor_state_t *const state)
{
  attron(A_BOLD);
  mvprintw(state->render_params.height - 2, 0, "%d:%d\t%s\t%u",
           line_number(state->point),
           column(state->point),
           state->mode->name,
           state->render_params.point_locator);
  attroff(A_BOLD);
}


editor_state_t* new_editor_state()
{
  editor_state_t *state = NULL;
  buffer_iter_t *const buffer = new_buffer();
  buffer_iter_t *point = NULL;

  if (buffer && copy_buffer_iter(buffer, &point) == SUCCESS) {
    state = calloc(sizeof(editor_state_t), 1);
    if (state) {
      state->point = point;
      state->terminate = false;
      switch_mode(state, NORMAL);
    } else {
      destroy_buffer(buffer);
    }
  }
  return state;
}


void destroy_editor_state(editor_state_t *state)
{
  state->point = NULL;
  destroy_buffer(state->point);
  free(state);
}


error_t insert_mode_handler(event_t event, struct editor_state_t *const state)
{
  switch (event) {
  case KEY_ESCAPE:
    switch_mode(state, NORMAL);
    break;
  case 127: // TODO Why is this not KEY_BACKSPACE?
    delete_character_at_point(state->point);
    break;
  default:
    return insert_character_at_point(state->point, event);
    break;
  }
  return SUCCESS;
}


error_t normal_mode_handler(event_t event, struct editor_state_t *const state)
{
  switch (event) {
  case 'i':
    switch_mode(state, INSERT);
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
    open_line(state->point);
    // TODO tidy
    move_cursor_down(state);
    switch_mode(state, INSERT);
    break;
  case 'q':
    state->terminate = true;
    break;
  default:
    break;
  }
  return SUCCESS;
}


void open_line(buffer_iter_t *const iter)
{
  append_line_at_point(iter);
  /* move_iter_down_line(iter); */
  move_to_beginning_of_line(iter);
}



bool should_quit(const editor_state_t *const state)
{
  return state->terminate;
}


void switch_mode(editor_state_t *const state, editor_mode_t mode)
{
  state->mode = get_mode_handle(mode);
}


/* Rendering questions */

size_t terminal_lines(const render_params_t *const params,
                      const buffer_iter_t *const iter)
{
  return chars_in_line(iter) / params->width + 1;
}


size_t locate_start_of_render(const render_params_t *const params,
                              buffer_iter_t *render_point)
{
  size_t locator = params->point_locator;

  while (!is_first_line(render_point)) {
    move_iter_up_line(render_point);

    if (terminal_lines(params, render_point) > locator) {
      break;
    }

    locator -= terminal_lines(params, render_point);
  }

  if (!is_first_line(render_point)) {
    move_iter_down_line(render_point);
  }

  return locator;
}


void move_cursor_up(editor_state_t *const state)
{
  if (!is_first_line(state->point)) {
    move_iter_up_line(state->point);

    const size_t lines_for_current =
      terminal_lines(&state->render_params, state->point);

    if (lines_for_current > state->render_params.point_locator) {
      state->render_params.point_locator = 0;
    } else {
      state->render_params.point_locator -= lines_for_current;
    }
  }
}


void move_cursor_down(editor_state_t *const state)
{
  if (!is_last_line(state->point)) {
    const size_t lines_for_current =
      terminal_lines(&state->render_params, state->point);

    // This needs a better bound
    state->render_params.point_locator =
      min(state->render_params.height - 3,
          state->render_params.point_locator + lines_for_current);

    move_iter_down_line(state->point);
  }
}
