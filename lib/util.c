#include "util.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

static FILE *open_file(const char *mode);

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

char *get_file_path() {
    struct passwd *pw = getpwuid(getuid());
    if (pw == NULL) {
        perror("getpwuid");
        return NULL;
    }
    
    size_t path_length = strlen(pw->pw_dir) + strlen(FILE_NAME) + 2; // 1 for '/' and 1 for null terminator
    char *file_path = (char *) malloc(path_length);
    
    if (file_path == NULL) {
        perror("allocation");
        return NULL;
    }

    snprintf(file_path, path_length, "%s/%s", pw->pw_dir, FILE_NAME);
    return file_path;
}

FILE *get_file_writer() {
    return open_file("w");
}

FILE *get_file_reader() {
    return open_file("r");
}

static FILE *open_file(const char *mode) {
    char *path = get_file_path();
    FILE *f = fopen(path, mode);

    free(path);

    if (f == NULL)
        PERROR("Cannot open calendar file.");
    
    return f;
}

