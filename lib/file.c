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
 *     3 = once
 * 
 * data:
 *     daily:   hour|minute
 *     weekly:  week_day|hour|minute
 *     monthly: month_day|hour|minute
 *     yearly:  month_day|month|hour|minute
 *     once:    month_day|month|year|hour|minute
 * 
 * Descriptions must not have the separator character.
 */
static bool parse_line(char *line, Alarm *out_alarm) {
    char *desc = strtok(line, SEPARATOR);

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
                case 3: id = ALARM_ONCE;    break;
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
                    else if (count >= 3) {
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

                    break;
                }

                case ALARM_WEEKLY: {
                    uint8_t week_day, hours, minutes;

                    if (count == 1 && sscanf(token, "%hhd", &week_day) != 1)
                        return false;
                    else if (count == 2 && sscanf(token, "%hhd", &hours) != 1)
                        return false;
                    else if (count == 3 && sscanf(token, "%hhd", &minutes) != 1)
                        return false;
                    else if (count >= 3) {
                        *out_alarm = (Alarm) {
                            .description = desc,
                            .type = (AlarmType) {
                                .id = id,
                                .alarm.weekly = {
                                    .week_day = week_day,
                                    .hour = (Hour) {
                                        .hours = hours,
                                        .minutes = minutes,
                                    },
                                }
                            }
                        };

                        return true;
                    }

                    break;
                }

                case ALARM_MONTHLY: {
                    uint8_t month_day, hours, minutes;
                    
                    if (count == 1 && sscanf(token, "%hhd", &month_day) != 1)
                        return false;
                    else if (count == 2 && sscanf(token, "%hhd", &hours) != 1)
                        return false;
                    else if (count == 3 && sscanf(token, "%hhd", &minutes) != 1)
                        return false;
                    else if (count >= 4) {
                        *out_alarm = (Alarm) {
                            .description = desc,
                            .type = (AlarmType) {
                                .id = id,
                                .alarm.monthly = {
                                    .month_day = month_day,
                                    .hour = (Hour) {
                                        .hours = hours,
                                        .minutes = minutes,
                                    },
                                }
                            }
                        };

                        return true;
                    }

                    break;
                }

                case ALARM_YEARLY: {
                    uint8_t month_day, month, hours, minutes;
                    
                    if (count == 1 && sscanf(token, "%hhd", &month_day) != 1)
                        return false;
                    else if (count == 2 && sscanf(token, "%hhd", &month) != 1)
                        return false;
                    else if (count == 3 && sscanf(token, "%hhd", &hours) != 1)
                        return false;
                    else if (count == 4 && sscanf(token, "%hhd", &minutes) != 1)
                        return false;
                    else if (count >= 5) {
                        *out_alarm = (Alarm) {
                            .description = desc,
                            .type = (AlarmType) {
                                .id = id,
                                .alarm.yearly = {
                                    .month_day = month_day,
                                    .month = month,
                                    .hour = (Hour) {
                                        .hours = hours,
                                        .minutes = minutes,
                                    },
                                }
                            }
                        };

                        return true;
                    }

                    break;
                }

                case ALARM_ONCE: {
                    uint8_t month_day, month, hours, minutes;
                    uint32_t year;
                    
                    if (count == 1 && sscanf(token, "%hhd", &month_day) != 1)
                        return false;
                    else if (count == 2 && sscanf(token, "%hhd", &month) != 1)
                        return false;
                    else if (count == 3 && sscanf(token, "%d", &year) != 1)
                        return false;
                    else if (count == 4 && sscanf(token, "%hhd", &hours) != 1)
                        return false;
                    else if (count == 5 && sscanf(token, "%hhd", &minutes) != 1)
                        return false;
                    else if (count >= 6) {
                        *out_alarm = (Alarm) {
                            .description = desc,
                            .type = (AlarmType) {
                                .id = id,
                                .alarm.yearly = {
                                    .month_day = month_day,
                                    .month = month,
                                    .hour = (Hour) {
                                        .hours = hours,
                                        .minutes = minutes,
                                    },
                                }
                            }
                        };

                        return true;
                    }

                    break;
                }
            }
        }

        count++;
    }

    return true;
}

