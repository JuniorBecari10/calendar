#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "lib/operations.h"
#include "lib/util.h"

int main(int argc, char *argv[]) {
    switch (argc) {
        case 1: {
            print_current_month();
            return 0;
        }
        
        case 2: {
            // now, help, version, watch. default: print year
            char *option = argv[1];

            if (strcasecmp(option, "now") == 0)
                print_now();
            else if (strcasecmp(option, "help") == 0)
                help();
            else if (strcasecmp(option, "version") == 0)
                version();
            else if (strcasecmp(option, "watch") == 0)
                watch();
            else {
                int32_t year;

                if (sscanf(option, "%d", &year) != 1)
                    ERROR("Invalid year/option.");

                if (year <= 0)
                    ERROR("Year must be greater than 0.");
 
                print_year(year);
            }

            return 0;
        }

        case 3: {
            // import, export, remove. default: print month
            // for now, these options won't be implemented

            int8_t month;
            int32_t year;

            if (sscanf(argv[1], "%hhd", &month) != 1)
                ERROR("Invalid month/option.");

            if (sscanf(argv[2], "%d", &year) != 1)
                ERROR("Invalid year/option.");

            if (month < 0 || month > 12)
                ERROR("Month must be between 1 and 12.");

            if (year <= 0)
                ERROR("Year must be greater than 0.");

            print_month(month, year);
            return 0;
        }

        default: {
            if (strcasecmp(argv[1], "alarm") == 0) {
                char *option = argv[2];

                if (strcasecmp(option, "add") == 0) {
                    // desc <daily ... | weekly ... | monthly ... | yearly | unique ...>

                    if (argc < 5)
                        ERROR("Description and options must be specified.");

                    char *description = argv[3];

                    if (strchr(description, SEPARATOR_CHAR) != NULL)
                        ERROR("The description must not contain '" SEPARATOR "\'.");

                    char *option = argv[4];

                    if (strcasecmp(option, "daily") == 0) {
                        // <hh:mm>
                        
                        if (argc < 6)
                            ERROR("Please specify the hour.");

                        char *hour_str = argv[5];
                        uint8_t hours, minutes;

                        if (sscanf(hour_str, "%hhd:%hhd", &hours, &minutes) != 2)
                            ERROR("Please specify the hour correctly: 'hh:mm'.");

                        Hour hour = (Hour) {
                            .hours = hours,
                            .minutes = minutes,
                        };

                        if (!is_valid_hour(hour))
                            ERROR("Please specify a valid hour.");

                        Alarm alarm = (Alarm) {
                            .description = description,
                            .type = (AlarmType) {
                                .id = ALARM_DAILY,
                                .alarm.daily = {
                                    .hour = hour,
                                },
                            },
                        };

                        alarm_add(alarm);
                    }

                    else if (strcasecmp(option, "weekly") == 0) {
                        // <day: index/name> <hh:mm>
                        char *week_days[] = { "sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday" };
                        const size_t length = 7;

                        if (argc < 7)
                            ERROR("Please specify the day of the week and the hour.");

                        char *week_day_str = argv[5];
                        char *hour_str = argv[6];

                        // TODO: get by index as well
                        size_t week_day;
                        if ((week_day = get_index_str(week_days, length, week_day_str)) == SIZE_MAX)
                            ERROR("Please specify a valid day of the week.");

                        uint8_t hours, minutes;

                        if (sscanf(hour_str, "%hhd:%hhd", &hours, &minutes) != 2)
                            ERROR("Please specify the hour correctly: 'hh:mm'.");

                        Hour hour = (Hour) {
                            .hours = hours,
                            .minutes = minutes,
                        };

                        if (!is_valid_hour(hour))
                            ERROR("Please specify a valid hour.");

                        Alarm alarm = (Alarm) {
                            .description = description,
                            .type = (AlarmType) {
                                .id = ALARM_WEEKLY,
                                .alarm.weekly = {
                                    .week_day = (uint8_t) week_day,
                                    .hour = hour,
                                },
                            },
                        };

                        alarm_add(alarm);
                    }

                    else if (strcasecmp(option, "monthly") == 0) {
                        // <day> <hh:mm>
                        if (argc < 7)
                            ERROR("Please specify the day of the month and the hour.");

                        char *month_day_str = argv[5];
                        char *hour_str = argv[6];

                        uint8_t month_day, hours, minutes;
                        bool clamp = false; // default
                        
                        if (argc >= 8) {
                            char *check_clamp = argv[7];

                            if (strcasecmp(check_clamp, "--clamp") == 0)
                                clamp = true;
                        }
                        
                        if (sscanf(month_day_str, "%hhd", &month_day) != 1)
                            ERROR("Please specify a valid day of the month.");
                        
                        if (sscanf(hour_str, "%hhd:%hhd", &hours, &minutes) != 2)
                            ERROR("Please specify the hour correctly: 'hh:mm'.");
                        
                        Hour hour = (Hour) {
                            .hours = hours,
                            .minutes = minutes,
                        };

                        if (!is_valid_hour(hour))
                            ERROR("Please specify a valid hour.");

                        if (month_day > 31)
                            ERROR("Please specify a valid day of the month.");
                        else if (month_day > 28) {
                            if (!clamp)
                                ERROR("Cannot set this day of the month, because not all months have it.\nBut you can clamp the day (i.e. ring the alarm on the last day of the month, if it doesn't have the day), by specifying '--clamp' as the last argument.");
                        }
                        else if (clamp)
                            WARN("The '--clamp' flag isn't necessary, since all months have at least 28 days.");
                            
                        
                        Alarm alarm = (Alarm) {
                            .description = description,
                            .type = (AlarmType) {
                                .id = ALARM_MONTHLY,
                                .alarm.monthly = {
                                    .month_day = month_day,
                                    .hour = hour,
                                    .clamp = clamp,
                                },
                            },
                        };

                        alarm_add(alarm);
                    }

                    else if (strcasecmp(option, "yearly") == 0) {
                        // <month: index/name> <day> <hh:mm>
                        char *months[] = { "january", "february", "march", "april", "may", "june",
                                           "july", "august", "october", "november", "december" };
                        const size_t length = 12;
                        
                        if (argc < 8)
                            ERROR("Please specify the month, the day of the month and the hour.");

                        char *month_str = argv[5];
                        char *month_day_str = argv[6];
                        char *hour_str = argv[7];
                        
                        size_t month;
                        if ((month = get_index_str(months, length, month_str)) == SIZE_MAX)
                            ERROR("Please specify a valid month.");
                    }
                    
                    else if (strcasecmp(option, "unique") == 0) {
                        // <year> <month: index/name> <day> <hh:mm>
                    }

                    else ERROR("Invalid time interval.");
                }
 
                else if (strcasecmp(option, "edit") == 0) {

                }

                else if (strcasecmp(option, "list") == 0) {

                }

                else if (strcasecmp(option, "remove") == 0) {

                }

                return 0;
            }

            else ERROR("Invalid option.");
        }
    }
}

