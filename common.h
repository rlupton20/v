#pragma once


#define min(a, b) (a) < (b) ? (a) : (b)


typedef enum error_t {
                      SUCCESS = 0,
                      ALLOC_ERROR = 1
} error_t;
