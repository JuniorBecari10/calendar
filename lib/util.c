#include "util.h"
#include "list.h"
#include "operations.h"
#include "calendar.h"

#include <ctype.h>
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
    const char *home = getenv("HOME"); // For Unix
    
    if (!home)
        home = getenv("USERPROFILE"); // Windows fallback
    
    size_t path_length = strlen(home) + strlen(FILE_NAME) + 2; // 1 for '/' and 1 for null terminator
    char *file_path = (char *) malloc(path_length);
    
    if (file_path == NULL) {
        perror("allocation");
        return NULL;
    }

    snprintf(file_path, path_length, "%s/%s", home, FILE_NAME);
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
    return random_number(0, MAX_LEN + 1);
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

bool alarm_types_equal(AlarmType* a, AlarmType* b) {
    if (a->id != b->id) {
        return false;
    }

    switch (a->id) {
        case ALARM_DAILY:
            return hours_equal(a->alarm.daily.hour, b->alarm.daily.hour);

        case ALARM_WEEKLY:
            return a->alarm.weekly.week_day == b->alarm.weekly.week_day &&
                   hours_equal(a->alarm.weekly.hour, b->alarm.weekly.hour);

        case ALARM_MONTHLY:
            return a->alarm.monthly.month_day == b->alarm.monthly.month_day &&
                   a->alarm.monthly.clamp == b->alarm.monthly.clamp &&
                   hours_equal(a->alarm.monthly.hour, b->alarm.monthly.hour);

        case ALARM_YEARLY:
            return a->alarm.yearly.month == b->alarm.yearly.month &&
                   a->alarm.yearly.month_day == b->alarm.yearly.month_day &&
                   a->alarm.yearly.clamp == b->alarm.yearly.clamp &&
                   hours_equal(a->alarm.yearly.hour, b->alarm.yearly.hour);

        case ALARM_ONCE:
            return a->alarm.once.year == b->alarm.once.year &&
                   a->alarm.once.month == b->alarm.once.month &&
                   a->alarm.once.month_day == b->alarm.once.month_day &&
                   hours_equal(a->alarm.once.hour, b->alarm.once.hour);
    }

    return false;
}

bool is_last_day_of_the_month(uint32_t year, uint8_t month, uint8_t day) {
    if (month < 1 || month > 12)
        return false;

    int32_t num_days_of_months[] = { -1, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    
    if (is_leap_year(year))
        num_days_of_months[2] = 29;

    return day == num_days_of_months[month];
}

bool ask_for_confirmation(char *prompt) {
    printf("%s (y/n): ", prompt);
    return tolower(getc(stdin)) == 'y';
}

DateComplete now_complete() {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);

    return (DateComplete) {
        .year = tm_info->tm_year + 1900,
        .month = tm_info->tm_mon + 1,
        .month_day = tm_info->tm_mday,
        .week_day = tm_info->tm_wday + 1,

        .hour = (HourSeconds) {
            .hours = tm_info->tm_hour,
            .minutes = tm_info->tm_min,
            .seconds = tm_info->tm_sec,
        },
    };
}

/*
 * Will ring today if the hour has not passed, and:
 *
 * - the alarm has a daily frequency;
 * - the alarm has a weekly frequency and today's day of the week matches with it;
 * - the alarm has a monthly frequency and today's day of the month matches with it
 *      (or today is the last day of the month and the day is clamp);
 * - the alarm has a yearly frequency and today's month and day of the month matches with it
 *      (or today is the last day of the month and the day is clamp);
 * - the alarm has a once frequency and today is the specified day.
 */
AlarmList get_alarms_to_ring_today(AlarmList list) {
    AlarmList res = new_alarm_list();
    DateComplete now = now_complete();

    LIST_ITER(a, list) {
        switch (a->type.id) {
            case ALARM_DAILY: {
                struct AlarmDaily daily = a->type.alarm.daily;
                if (!hour_has_passed(daily.hour, hour_seconds_to_hour(now.hour)))
                    push_alarm(&res, clone_alarm(a));
                break;
            }

            case ALARM_WEEKLY: {
                struct AlarmWeekly weekly = a->type.alarm.weekly;
                if (weekly.week_day == now.week_day
                    && !hour_has_passed(weekly.hour, hour_seconds_to_hour(now.hour)))
                    push_alarm(&res, clone_alarm(a));

                break;
            }

            case ALARM_MONTHLY: {
                struct AlarmMonthly monthly = a->type.alarm.monthly;
                if (((monthly.month_day == now.month_day)
                    || (monthly.clamp && is_last_day_of_the_month(now.year, now.month, now.month_day)))
                    && !hour_has_passed(monthly.hour, hour_seconds_to_hour(now.hour)))
                    push_alarm(&res, clone_alarm(a));

                break;
            }
            
            case ALARM_YEARLY: {
                struct AlarmYearly yearly = a->type.alarm.yearly;
                if (yearly.month == now.month
                    && ((yearly.month_day == now.month_day) ||
                        (yearly.clamp && is_last_day_of_the_month(now.year, now.month, now.month_day)))
                    && !hour_has_passed(yearly.hour, hour_seconds_to_hour(now.hour)))
                    push_alarm(&res, clone_alarm(a));
                
                break;
            }
            
            case ALARM_ONCE: {
                struct AlarmOnce once = a->type.alarm.once;
                if (once.month_day == now.month_day
                    && once.month == now.month
                    && once.year == now.year
                    && !hour_has_passed(once.hour, hour_seconds_to_hour(now.hour)))
                    push_alarm(&res, clone_alarm(a));

                break;
            }
        }
    }

    return res;
}

// must be the result of 'get_alarms_to_ring_today'.
AlarmList get_alarms_to_ring_now(AlarmList list_today) {
    DateComplete now = now_complete();
    AlarmList ring_now = new_alarm_list();
    
    LIST_ITER(a, list_today) {
        #define RING_CASE(name, field)                                                     \
            case ALARM_##name: {                                                           \
                if (hours_equal(hour_seconds_to_hour(now.hour), a->type.alarm.field.hour)) \
                    push_alarm(&ring_now, clone_alarm(a));                                 \
                                                                                           \
                break;                                                                     \
            }
        
        switch (a->type.id) {
            RING_CASE(DAILY, daily)
            RING_CASE(WEEKLY, weekly)
            RING_CASE(MONTHLY, monthly)
            RING_CASE(YEARLY, yearly)
            RING_CASE(ONCE, once)
        }
    }

    return ring_now;
}

Alarm clone_alarm(Alarm *alarm) {
    Alarm clone = *alarm;
    clone.description = duplicate_str(alarm->description);

    return clone;
}

void free_alarm(Alarm *alarm) {
    free(alarm->description);
}

char *duplicate_str(char *str) {
    size_t len = strlen(str) + 1;
    char* copy = malloc(len);

    return copy ? memcpy(copy, str, len) : NULL;
}

