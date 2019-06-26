#pragma once

#include "buffer.h"
#include "common.h"

error_t
read_file_into_editor(buffer_iter_t* const iter, const char* const filename);
error_t
write_buffer_to_disk(buffer_iter_t* const iter, const char* const filename);
