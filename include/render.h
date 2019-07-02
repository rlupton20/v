#pragma once

#include <common.h>

/*
 * Render the current editor state to sceen.
 */
void
render(const editor_state_t* const state, render_params_t* const params);

/*
 * Update the render parameters.
 */
void
update_render_params(render_params_t* const render_params);
