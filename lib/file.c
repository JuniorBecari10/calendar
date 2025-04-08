#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "list.h"
#include "operations.h"
#include "util.h"

#define MAX_LINE 2048

static bool parse_line(char *line, Alarm *out_alarm);
static void write_hour(FILE *f, Hour hour);

bool parse_file(AlarmList *out_list) {
    FILE *file = get_file_reader();
    *out_list = new_alarm_list();
    
    if (!file) {
        // the file doesn't exist yet or some another error.
        // either case, we'll return an empty list.
        // and return true in this function, so that the caller can continue its control flow.

        return true;
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file)) {
        if (strchr(line, '\n') == NULL) {
            // line is too long to be parsed. the program may not function correctly.
            PERROR("Parse error: line too long to be parsed.");
            free_alarm_list(out_list);

            return false;
        }

        line[strcspn(line, "\n")] = '\0'; // properly terminate the string.
        
        Alarm alarm;
        if (!parse_line(line, &alarm)) {
            // error during line parse. the program may not function correctly.
            PERROR("Parse error: could not parse calendar file.");
            free_alarm_list(out_list);

            return false;
        }

        push_alarm(out_list, alarm);
    }

    fclose(file);
    return true;
}

bool write_to_file(AlarmList list) {
    FILE *w = get_file_writer();

    LIST_ITER(a, list) {
        // desc|id|type|
        fprintf(w, "%s|%hd|%hhd|", a->description, a->id, (uint8_t) a->type.id);

        switch (a->type.id) {
            case ALARM_DAILY: {
                struct AlarmDaily daily = a->type.alarm.daily;
                write_hour(w, daily.hour);

                break;
            }

            case ALARM_WEEKLY: {
                struct AlarmWeekly weekly = a->type.alarm.weekly;
                
                fprintf(w, "%hhd|", weekly.week_day);
                write_hour(w, weekly.hour);

                break;
            }

            case ALARM_MONTHLY: {
                struct AlarmMonthly monthly = a->type.alarm.monthly;
                
                fprintf(w, "%hhd|%hhd|", monthly.month_day, monthly.clamp);
                write_hour(w, monthly.hour);

                break;
            }

            case ALARM_YEARLY: {
                struct AlarmYearly yearly = a->type.alarm.yearly;
                
                fprintf(w, "%hhd|%hhd|%hhd|", yearly.month_day, yearly.month, yearly.clamp);
                write_hour(w, yearly.hour);

                break;
            }

            case ALARM_ONCE: {
                struct AlarmOnce once = a->type.alarm.once;
                
                fprintf(w, "%hhd|%hhd|%hhd|", once.month_day, once.month, once.year);
                write_hour(w, once.hour);

                break;
            }
        }

        fprintf(w, "\n");
    }

    fclose(w);
    return true;
}

/*
 * Separator: |
 * desc|id|<type>|<data>
 *
 * type:
 *     0 = daily
 *     1 = weekly
 *     2 = monthly
 *     3 = yearly
 *     4 = once
 * 
 * data:
 *     daily:   hour|minute
 *     weekly:  week_day|hour|minute
 *     monthly: month_day|clamp|hour|minute
 *     yearly:  month_day|month|clamp|hour|minute
 *     once:    month_day|month|year|hour|minute
 * 
 * Descriptions must not have the separator character.
 */
static bool parse_line(char *line, Alarm *out_alarm) {
    char *desc = strtok(line, SEPARATOR);
    if (!desc)
        return false;
    
    char *id_str = strtok(NULL, SEPARATOR);
    if (!id_str)
        return false;

    Id alarm_id = atoi(id_str);

    char *type_str = strtok(NULL, SEPARATOR);
    if (!type_str)
        return false;

    uint8_t parsed_id;
    if (sscanf(type_str, "%hhu", &parsed_id) != 1)
        return false;

    AlarmTypeId id;
    switch (parsed_id) {
        case 0: id = ALARM_DAILY;   break;
        case 1: id = ALARM_WEEKLY;  break;
        case 2: id = ALARM_MONTHLY; break;
        case 3: id = ALARM_YEARLY;  break;
        case 4: id = ALARM_ONCE;    break;
        
        default:
            return false; // Invalid type
    }

    uint8_t count = 0;
    int32_t values[5] = {0}; // because it may contain 'year'
    
    char *token;
    while ((token = strtok(NULL, SEPARATOR)) != NULL) {
        if (count >= 5)
            return false; // Too many tokens

        if (sscanf(token, "%d", &values[count]) != 1)
            return false;

        count++;
    }

    if ((id == ALARM_DAILY   && count != 2) ||
        (id == ALARM_WEEKLY  && count != 3) ||
        (id == ALARM_MONTHLY && count != 4) ||
        (id == ALARM_YEARLY  && count != 5) ||
        (id == ALARM_ONCE    && count != 5))
        return false; // Invalid number of tokens

    *out_alarm = (Alarm) {
        .description = duplicate_str(desc),
        .id = alarm_id,
        .type = {
            .id = id,
        },
    };

    #define HOUR(h, m) (Hour) { .hours = (uint8_t) h, .minutes = (uint8_t) m, }

    switch (id) {
        case ALARM_DAILY: {
            out_alarm->type.alarm.daily = (struct AlarmDaily) {
                .hour = HOUR(values[0], values[1]),
            };

            break;
        }

        case ALARM_WEEKLY: {
            out_alarm->type.alarm.weekly = (struct AlarmWeekly) {
                .week_day = (uint8_t) values[0],
                .hour = HOUR(values[1], values[2]),
            };

            break;
        }

        case ALARM_MONTHLY: {
            out_alarm->type.alarm.monthly = (struct AlarmMonthly) {
                .month_day = (uint8_t) values[0],
                .clamp = (bool) values[1],
                .hour = HOUR(values[2], values[3]),
            };

            break;
        }

        case ALARM_YEARLY: {
            out_alarm->type.alarm.yearly = (struct AlarmYearly) {
                .month_day = (uint8_t) values[0],
                .month = (uint8_t) values[1],
                .clamp = (bool) values[2],
                .hour = HOUR(values[3], values[4]),
            };

            break;
        }

        case ALARM_ONCE: {
            out_alarm->type.alarm.once = (struct AlarmOnce) {
                .month_day = (uint8_t) values[0],
                .month = (uint8_t) values[1],
                .year = values[2],
                .hour = HOUR(values[3], values[4]),
            };

            break;
        }
    }

    #undef HOUR
    return true;
}

static void write_hour(FILE *f, Hour hour) {
    fprintf(f, "%hhd|%hhd", hour.hours, hour.minutes);
}

