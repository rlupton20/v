#include <ncurses.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "common.h"
#include "buffer.h"


typedef int event_t;


typedef struct editor_state_t {
  buffer_iter_t *buffer;
  buffer_iter_t *point;
  bool terminate;
} editor_state_t;


/* Functions for working with editor state */
editor_state_t* new_editor_state();
void destroy_editor_state(editor_state_t *state);


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
    return insert_character_at_point(state->point, event);
  }
  return SUCCESS;
}


void render(const editor_state_t *const state)
{
  clear();
  render_modeline(state);
  mvprintw(0, 0, "%s", current_line(state->point));
  refresh();
}


void render_modeline(const editor_state_t *const state)
{
  attron(A_BOLD);
  mvprintw(30, 0, "0:%d", column(state->point));
  attroff(A_BOLD);
}


bool should_quit(const editor_state_t *const state)
{
  return state->terminate;
}


editor_state_t* new_editor_state()
{
  editor_state_t *state = NULL;
  buffer_iter_t *const buffer = new_buffer();
  if (buffer) {
    state = calloc(sizeof(editor_state_t), 1);
    if (state) {
      state->buffer = buffer;
      state->point = buffer;
      state->terminate = false;
    } else {
      destroy_buffer(buffer);
    }
  }
  return state;
}


void destroy_editor_state(editor_state_t *state)
{
  state->point = NULL;
  destroy_buffer(state->buffer);
  free(state);
}
