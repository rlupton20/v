#include <files.h>
#include <mode.h>
#include <state.h>

/*
 * Execute the command in the command buffer.
 */
void
execute_command(editor_state_t* const state);

error_t
command_mode_handler(event_t event, struct editor_state_t* const state)
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
