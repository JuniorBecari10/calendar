#ifndef UTIL_H
#define UTIL_H

#include "operations.h"
#include <stdbool.h>
#include <stddef.h>

#define SEPARATOR "|"
#define SEPARATOR_CHAR '|'

#define ERROR(message)                 \
    do {                               \
        fprintf(stderr, "Error: " message "\n"); \
        return 1;                      \
    } while (0)

#define PERROR(message)                \
    do {                               \
        fprintf(stderr, "Error: " message "\n"); \
    } while (0)

#define WARN(message) fprintf(stderr, "Warning: " message "\n");

bool is_valid_hour(Hour hour);
size_t get_index_str(char *arr[], size_t size, char *value);

#endif
