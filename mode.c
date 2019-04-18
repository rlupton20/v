#include "common.h"
#include "mode.h"


static const mode_t normal_mode = (mode_t) {
                                            .name = "NORMAL",
                                            .handler = normal_mode_handler
};


static const mode_t insert_mode = (mode_t) {
                                            .name = "INSERT",
                                            .handler = insert_mode_handler
};


const mode_t* get_mode_handle(editor_mode_t mode)
{
  static const mode_t* editor_modes[] = {
                                         [NORMAL] = &normal_mode,
                                         [INSERT] = &insert_mode
  };

  return editor_modes[mode];
}