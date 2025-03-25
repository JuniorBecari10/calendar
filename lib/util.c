#include "util.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <strings.h>

bool is_valid_hour(Hour hour) {
    return hour.hours < 24 && hour.minutes < 60;
}

// we won't consider leap years
bool month_has_day(uint8_t month, uint8_t day) {
    if (month > 12) return false;

    int8_t num_days_of_months[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    return day <= num_days_of_months[month]; 
}
size_t get_index_str(char *arr[], size_t size, char *value) {
    size_t index = 0;

    for (; index < size; index++)
        if (strcasecmp(arr[index], value) == 0) break;

    return index == size ? SIZE_MAX : index;
}

