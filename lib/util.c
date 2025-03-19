#include "util.h"
#include <stdbool.h>

bool is_valid_hour(Hour hour) {
    return hour.hours < 24 && hour.minutes < 60;
}

