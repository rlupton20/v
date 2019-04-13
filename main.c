#include <ncurses.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "common.h"
#include "buffer.h"
#include "line_buffer.h"


typedef int event_t;


typedef struct editor_state_t {
  line_buffer_t *current_line;
  size_t column;
  bool terminate;
} editor_state_t;


/* Functions for working with editor state */
editor_state_t* new_editor_state();
void destroy_editor_state(editor_state_t *state);
error_t insert_at_point(editor_state_t *const state, const char c);


/* Functions for running the editor */
error_t update(const event_t event, editor_state_t *const state);
void render(const editor_state_t *const state);
void render_modeline(const editor_state_t *const state);
bool should_quit(const editor_state_t *const state);


int main(int argv, char *argc[])
{
  editor_state_t *state = new_editor_state();

  if (!state) {
    return 1;
  }

  initscr();
  noecho();

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
  switch (event) {
  case 'q':
    state->terminate = true;
    break;
  default:
    return insert_at_point(state, event);
  }
  return SUCCESS;
}


void render(const editor_state_t *const state)
{
  clear();
  render_modeline(state);
  mvprintw(0, 0, "%s", state->current_line->buffer);
  refresh();
}


void render_modeline(const editor_state_t *const state)
{
  attron(A_BOLD);
  mvprintw(30, 0, "0:%d", state->column);
  attroff(A_BOLD);
}


bool should_quit(const editor_state_t *const state)
{
  return state->terminate;
}


editor_state_t* new_editor_state()
{
  editor_state_t *state = NULL;
  line_buffer_t *line_buffer = new_line_buffer(DEFAULT_LINE_BUFFER_LENGTH);
  if (line_buffer) {
    state = calloc(sizeof(editor_state_t), 1);
    if (state) {
      state->current_line = line_buffer;
      state->column = 0;
      state->terminate = false;
    } else {
      free(line_buffer);
    }
  }
  return state;
}


void destroy_editor_state(editor_state_t *state)
{
  free(state->current_line);
  free(state);
}


error_t insert_at_point(editor_state_t *const state, const char c)
{
  return insert_character(state->current_line, c, state->column++);
}

