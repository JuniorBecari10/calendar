#ifndef UTIL_H
#define UTIL_H

#include "operations.h"
#include "list.h"

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

#define SEPARATOR "|"
#define SEPARATOR_CHAR '|'

#define MAX_DESCRIPTION_LENGTH 1024

// should not be used alone, because it doesn't have the path included.
#define FILE_NAME ".calendar"

#define ERROR(message)                           \
    do {                                         \
        fprintf(stderr, "Error: " message "\n"); \
        return 1;                                \
    } while (0)

#define ERRORR(message)                          \
    do {                                         \
        fprintf(stderr, "Error: " message "\n"); \
        return;                                  \
    } while (0)

#define PERROR(message)                          \
    do {                                         \
        fprintf(stderr, "Error: " message "\n"); \
    } while (0)

#define WARN(message)                            \
    fprintf(stderr, "Warning: " message "\n");

#define CLEAR() printf("\033[H\033[J")

bool is_valid_hour(Hour hour);
bool month_has_day(uint8_t month, uint8_t day);
size_t get_index_str(char *arr[], size_t size, char *value);

// must be freed.
char *get_file_path();

// must be 'fclose'd.
FILE *get_file_writer();
FILE *get_file_reader();

int random_number(int min, int max);

Id random_id();
Id random_unique_id(AlarmList *list);

bool date_has_passed(Date then, Date now);
bool hour_has_passed(Hour then, Hour now);

Hour hour_seconds_to_hour(HourSeconds hour);
bool hours_equal(Hour a, Hour b);
bool is_last_day_of_the_month(uint32_t year, uint8_t month, uint8_t day);

bool ask_for_confirmation(char *prompt);

Date now();
DateComplete now_complete();

AlarmList get_alarms_to_ring_today(AlarmList list);
AlarmList get_alarms_to_ring_now(AlarmList list_today);

#endif
