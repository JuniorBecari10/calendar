#include "util.h"
#include "list.h"
#include "operations.h"

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
#include <time.h>

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

    // if 'f' is 'NULL', don't do anything
    return f;
}

int random_number(int min, int max) {
    return min + (rand() % (max - min));
}

Id random_id() {
    return random_number(1000, MAX_LEN);
}

// TODO: make the ID a hash of the alarm, constrained into 4 digits
Id random_unique_id(AlarmList *list) {
    Id ret = random_id();
    bool changed = false;

    while (true) {
        changed = false;
        
        for (Alarm *a = list->list; (size_t) (a - list->list) < list->len; a++) {
            if (a->id == ret) {
                ret = random_id();
                changed = true;

                break;
            }
        }

        if (!changed)
            return ret;
    }
}

bool date_has_passed(Date then, Date now) {
    // Compare the years first
    if (now.year > then.year)
        return true;
    else if (now.year < then.year)
        return false;

    // If years are equal, compare the months
    if (now.month > then.month)
        return true;
    else if (now.month < then.month)
        return false;

    // If months are equal, compare the days
    if (now.month_day > then.month_day)
        return true;
    else if (now.month_day < then.month_day)
        return false;

    // Check the hours.
    return hour_has_passed(then.hour, now.hour);
}

bool hour_has_passed(Hour then, Hour now) {
    if (now.hours > then.hours)
        return true;
    else if (now.hours < then.hours)
        return false;
    
    // If hours are equal, compare the minutes
    if (now.minutes > then.minutes)
        return true;

    // If none of the above are greater, the hour has not passed
    return false;
}

Hour hour_seconds_to_hour(HourSeconds hour) {
    return (Hour) {
        .hours = hour.hours,
        .minutes = hour.minutes,
    };
}

Date now() {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);

    return (Date) {
        .year = tm_info->tm_year + 1900,
        .month = tm_info->tm_mon + 1,
        .month_day = tm_info->tm_mday,

        .hour = (Hour) {
            .hours = tm_info->tm_hour,
            .minutes = tm_info->tm_min,
        },
    };
}

bool hours_equal(Hour a, Hour b) {
    return a.hours == b.hours
        && a.minutes == b.minutes;
}

DateComplete now_complete() {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);

    return (DateComplete) {
        .year = tm_info->tm_year + 1900,
        .month = tm_info->tm_mon + 1,
        .month_day = tm_info->tm_mday,
        .week_day = tm_info->tm_wday,

        .hour = (HourSeconds) {
            .hours = tm_info->tm_hour,
            .minutes = tm_info->tm_min,
            .seconds = tm_info->tm_sec,
        },
    };
}

