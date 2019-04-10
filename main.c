#include <ncurses.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#define min(a, b) (a) < (b) ? (a) : (b)
#define DEFAULT_LINE_BUFFER_LENGTH 120


// The most a line buffer can expand by
const size_t max_buffer_growth = 240;


typedef enum error_t {
              SUCCESS = 0,
              ALLOC_ERROR = 1
} error_t;


typedef int event_t;


typedef struct line_buffer_t {
  size_t length;
  char *buffer;
} line_buffer_t;


typedef struct editor_state_t {
  line_buffer_t *current_line;
  size_t column;
  bool terminate;
} editor_state_t;


/* Functions for line buffers */
line_buffer_t* new_line_buffer(size_t capacity);
void destroy_line_buffer(line_buffer_t *line_buffer);
error_t insert_character(line_buffer_t *const line_buffer,
                      const char c,
                      const size_t ix);
error_t grow_buffer(line_buffer_t *const line_buffer);


/* Functions for working with editor state */
editor_state_t* new_editor_state();
void destroy_editor_state(editor_state_t *state);
error_t insert_character_at_point(editor_state_t *const state, const char c);


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
    return insert_character_at_point(state, event);
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


line_buffer_t* new_line_buffer(const size_t capacity)
{
  line_buffer_t *line_buffer = NULL;
  char *const buffer = calloc(sizeof(char), capacity + 1);
  if (buffer) {
    line_buffer = calloc(sizeof(line_buffer_t), 1);
    if (line_buffer) {
      line_buffer->length = capacity;
      line_buffer->buffer = buffer;
    } else {
      free(buffer);
    }
  }
  return line_buffer;
}


void destroy_line_buffer(line_buffer_t *line_buffer) {
  free(line_buffer->buffer);
  free(line_buffer);
}


error_t grow_buffer(line_buffer_t *const line_buffer) {
  const size_t new_size =
    min(max_buffer_growth + line_buffer->length, 2 * line_buffer->length) + 1;

  char* const new_buffer = realloc(line_buffer->buffer, new_size);
  if (new_buffer) {
    memset(new_buffer + line_buffer->length, 0, new_size - line_buffer->length);
    line_buffer->buffer = new_buffer;
    line_buffer->length = new_size - 1;
    return SUCCESS;
  } else {
    return ALLOC_ERROR;
  }
  return SUCCESS;
}


error_t insert_character(line_buffer_t *const line_buffer,
                      const char c,
                      const size_t ix)
{
  if (ix < line_buffer->length) {
    (line_buffer->buffer)[ix] = c;
    return SUCCESS;
  } else {
    const error_t grow = grow_buffer(line_buffer);
    return grow == SUCCESS ?
      insert_character(line_buffer, c, ix) : grow;
  }
  return SUCCESS;
}


error_t insert_character_at_point(editor_state_t *const state, const char c)
{
  return insert_character(state->current_line, c, state->column++);
}

