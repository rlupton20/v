#include <ncurses.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "common.h"
#include "buffer.h"


struct editor_state_t {
  buffer_iter_t *buffer;
  buffer_iter_t *point;
  mode_t *mode;
  bool terminate;
};


/* Functions for working with editor state */
editor_state_t* new_editor_state();
void destroy_editor_state(editor_state_t *state);


/* Functions for running the editor */
error_t update(const event_t event, editor_state_t *const state);
void render(const editor_state_t *const state);
void render_modeline(const editor_state_t *const state);
bool should_quit(const editor_state_t *const state);


static mode_t normal_mode = (mode_t) {
                                      .name = "NORMAL",
                                      .handler = normal_mode_handler
};


static mode_t insert_mode = (mode_t) {
                                      .name = "INSERT",
                                      .handler = insert_mode_handler
};


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
  return (state->mode->handler)(event, state);
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
  mvprintw(30, 0, "0:%d\t%s", column(state->point), state->mode->name);
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
      state->mode = &normal_mode;
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


error_t insert_mode_handler(event_t event, struct editor_state_t *const state)
{
  switch (event) {
  case KEY_ESCAPE:
    state->mode = &normal_mode;
    break;
  default:
    return insert_character_at_point(state->point, event);
  }
  return SUCCESS;
}


error_t normal_mode_handler(event_t event, struct editor_state_t *const state)
{
  switch (event) {
  case 'i':
    state->mode = &insert_mode;
    break;
  case 'q':
    state->terminate = true;
    break;
  default:
    break;
  }
  return SUCCESS;
}
