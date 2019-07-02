#include <ncurses.h>

#include <buffer.h>
#include <render.h>
#include <state.h>

static const size_t modeline_lines = 2;

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

void
render(const editor_state_t* const state, render_params_t* const render_params)
{
  buffer_iter_t* render_point;
  copy_buffer_iter(state->point, &render_point);

  size_t current = 0;
  size_t row = 0;

  erase();
  render_modeline(state, render_params);

  render_params->top_line = locate_start_of_render(render_params, render_point);
  while (current + modeline_lines < render_params->height) {
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
  mvprintw(render_params->height - modeline_lines,
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

size_t
terminal_lines(size_t terminal_width, const buffer_iter_t* const iter)
{
  return chars_in_line(iter) / terminal_width + 1;
}

size_t
locate_start_of_render(const render_params_t* const params,
                       buffer_iter_t* render_point)
{
  const size_t mintop = min(params->top_line, line_number(render_point));

  for (size_t depth =
         terminal_lines(params->width, render_point) + modeline_lines;
       line_number(render_point) != mintop;
       depth += terminal_lines(params->width, render_point)) {

    if (depth + terminal_lines(params->width, render_point) > params->height) {
      break;
    } else {
      move_iter_up_line(render_point);
    }
  }

  return line_number(render_point);
}

void
update_render_params(render_params_t* const render_params)
{
  getmaxyx(stdscr, render_params->height, render_params->width);
}
