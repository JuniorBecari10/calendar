#include "list.h"
#include "operations.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define MAX_LINE 2048

static bool parse_line(char *line, Alarm *out_alarm);

bool parse_file(AlarmList *out_list) {
    // for now
    FILE *file = fopen(".alarms", "r");
    *out_list = new_alarm_list();
    
    if (!file) {
        // the file doesn't exist yet or some another error.
        // either case, we'll return an empty list.

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
            PERROR("Parse error: could not parse alarms file.");
            free_alarm_list(out_list);

            return false;
        }

        push(out_list, alarm);
    }

    fclose(file);
    return true;
}

void write_to_file(AlarmList list) {

}

/*
 * Separator: |
 * desc|<type>|<data>
 *
 * type:
 *     0 = daily
 *     1 = weekly
 *     2 = monthly
 *     3 = unique
 * 
 * data:
 *     daily:   hour|minute
 *     weekly:  week_day|hour|minute
 *     monthly: month_day|hour|minute
 *     yearly:  month_day|month|hour|minute
 *     unique:  year|month|month_day|hour|minute
 * 
 * Descriptions must not have the separator character.
 */
static bool parse_line(char *line, Alarm *out_alarm) {
    char *desc = strtok(line, SEPARATOR);

    if (strchr(desc, SEPARATOR_CHAR) != NULL)
        return false;

    AlarmTypeId id = ALARM_DAILY;
    uint32_t count = 0;

    char *token;
    while ((token = strtok(NULL, SEPARATOR)) != NULL) {
        if (count == 0) {
            uint8_t parsed_id;
            if (sscanf(token, "%hhd", &parsed_id) != 1)
                return false;

            switch (parsed_id) {
                case 0: id = ALARM_DAILY;   break;
                case 1: id = ALARM_WEEKLY;  break;
                case 2: id = ALARM_MONTHLY; break;
                case 3: id = ALARM_UNIQUE;  break;
            }
        }

        else {
            switch (id) {
                case ALARM_DAILY: {
                    uint8_t hours, minutes;

                    if (count == 1 && sscanf(token, "%hhd", &hours) != 1)
                        return false;
                    else if (count == 2 && sscanf(token, "%hhd", &minutes) != 1)
                        return false;

                    *out_alarm = (Alarm) {
                        .description = desc,
                        .type = (AlarmType) {
                            .id = id,
                            .alarm.daily = {
                                .hour = (Hour) {
                                    .hours = hours,
                                    .minutes = minutes,
                                }
                            }
                        }
                    };

                    return true;
                }

                case ALARM_WEEKLY: {

                }

                case ALARM_MONTHLY: {

                }

                case ALARM_YEARLY: {

                }

                case ALARM_UNIQUE: {

                }
            }
        }

        count++;
    }

    return true;
}

