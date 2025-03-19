#include "util.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <strings.h>

bool is_valid_hour(Hour hour) {
    return hour.hours < 24 && hour.minutes < 60;
}

size_t get_index_str(char *arr[], size_t size, char *value) {
    size_t index = 0;

    while (index < size && strcasecmp(arr[index], value) != 0)
        ++index;

    return index == size ? SIZE_MAX : index;
}

