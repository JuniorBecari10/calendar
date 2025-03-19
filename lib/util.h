#ifndef UTIL_H
#define UTIL_H

#include "operations.h"
#include <stdbool.h>
#include <stddef.h>

#define SEPARATOR "|"
#define SEPARATOR_CHAR '|'

#define ERROR(message)                 \
    do {                               \
        fprintf(stderr, message "\n"); \
        return 1;                      \
    } while (0)

#define PERROR(message)                \
    do {                               \
        fprintf(stderr, message "\n"); \
    } while (0)

bool is_valid_hour(Hour hour);
size_t get_index_str(char *arr[], size_t size, char *value);

#endif
