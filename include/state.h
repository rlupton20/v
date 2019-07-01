#pragma once

#include <buffer.h>
#include <common.h>
#include <mode.h>

/*
 * editor_state_t is the structure containing the state of the
 * editor.
 */
struct editor_state_t
{
  buffer_iter_t* point;
  buffer_iter_t* command_buffer;
  const mode_t* mode;
  const char* filename;
  bool terminate;
};

/*
 * Create a new editor state structure.
 */
editor_state_t*
new_editor_state(const char* const filename);

/*
 * Clean up an editor state structure.
 */
void
destroy_editor_state(editor_state_t* state);

/*
 * Should quite returns false if and only if the editor has been
 * signalled to quit.
 */
bool
should_quit(const editor_state_t* const state);

/*
 * switch_mode sets the editor to operate in mode.
 */
void
switch_mode(editor_state_t* const state, editor_mode_t mode);

/*
 * Open a new line, and enter insert mode.
 */
error_t
open_line(editor_state_t* const state);

/*
 * Move the cursor up a line.
 */
void
move_cursor_up(editor_state_t* const state);

/*
 * Move the cursor down a line.
 */
void
move_cursor_down(editor_state_t* const state);

