#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <strings.h>
#include <stdbool.h>
#include <time.h>

#include "lib/calendar.h"
#include "lib/operations.h"
#include "lib/util.h"

static int scan_month_day(char *input, uint8_t *output, bool clamp);
static int scan_specific_month_day(char *input, uint8_t *output, uint8_t month, bool clamp);
static int scan_exact_month_day(char *input, uint8_t *output, uint8_t month, int32_t year);
static int scan_month(char *input, uint8_t *output);
static int scan_year(char *input, int32_t *output);
static int scan_hour(char *input, Hour *output);
static int scan_week_day(char *input, uint8_t *output);

static int parse_alarm_add(int len, char *args[]);
static int parse_alarm_edit(int len, char *args[]);
static int parse_alarm_list(char *filter);
static int parse_remove_alarm(char *id);

static int check_description(char *description);

int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (argc == 1) {
        print_current_month();
        return 0;
    }
    
    char *option = argv[1];
    
    if (strcasecmp(option, "now") == 0)
        print_now();
    
    else if (strcasecmp(option, "help") == 0)
        help();
    
    else if (strcasecmp(option, "version") == 0)
        version();
    
    else if (strcasecmp(option, "watch") == 0)
        watch();
    
    else if (strcasecmp(option, "import") == 0 && argc == 3)
        import_calendar(argv[2]);
    
    else if (strcasecmp(option, "export") == 0 && argc == 3)
        export_calendar(argv[2]);

    else if (strcasecmp(option, "list") == 0) {
        if (argc == 3) 
            parse_alarm_list(argv[2]);
        else
            alarm_list_all();
    }
    
    else if (strcasecmp(option, "alarm") == 0 && argc > 2) {
        char *suboption = argv[2];
       
        // adjust args to point to the relevant information
        argc -= 3;
        argv += 3;

        if (strcasecmp(suboption, "add") == 0 && argc >= 2)
            parse_alarm_add(argc, argv);
        
        else if (strcasecmp(suboption, "edit") == 0 && argc >= 1)
            parse_alarm_edit(argc, argv);
        
        else if (strcasecmp(suboption, "list") == 0 && argc >= 1)
            parse_alarm_list(argv[0]);
        
        else if (strcasecmp(suboption, "remove") == 0 && argc == 1)
            parse_remove_alarm(argv[0]);
        
        else
            ERROR("Invalid alarm option or incorrect number of arguments.");
    }

    else if (argc == 2) {
        int32_t year;
        
        if (scan_year(option, &year) != 0)
            return 1;

        print_year(year);
    }

    else if (argc == 3) {
        uint8_t month;
        int32_t year;

        if (scan_month(argv[1], &month) != 0 || scan_year(argv[2], &year) != 0)
            return 1;

        print_month(month, year);
    }

    else
        ERROR("Invalid option.");
    
    return 0;
}

static int scan_month_day(char *input, uint8_t *output, bool clamp) {
    if (sscanf(input, "%hhd", output) != 1)
        ERROR("Invalid day of the month.");

    if (*output < 1 || *output > 31)
        ERROR("Day of the month must be between 1 and 31.");

    if (*output > 28 && !clamp)
        ERROR("Not all months have this day of the month. Please specify the '--clamp' flag to clamp the day in months that don't have it.");

    return 0;
}

// checks if the day of a specific month exists in all years.
// all months have predefined days, except for February, which in leap years it has one more day. this is why we need 'clamp'.
// 'month' is base 1.
static int scan_specific_month_day(char *input, uint8_t *output, uint8_t month, bool clamp) {
    // we won't consider February with 29 days in leap years, because this checks in all years, and February has at least 28 days.
    static const uint8_t days_in_month[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31,
    };
    
    if (sscanf(input, "%hhd", output) != 1)
        ERROR("Invalid day of the month.");

    if (*output < 1 || *output > 31)
        ERROR("Day of the month must be between 1 and 31.");
    
    // check if clamp is necessary for February
    if (month == 2 && *output == 29) {
        if (!clamp)
            ERROR("Not all February months have 29 days. Use the '--clamp' flag to clamp the day.");
    }

    else if (*output > days_in_month[month - 1]) 
        ERROR("This month doesn't have this amount of days.");

    // TODO: change clamp to false in these cases,
    // which requires it to be a pointer instead.
    if (*output <= days_in_month[month - 1] && clamp)
        WARN("Clamping isn't necessary in this case.");

    return 0; 
}

// checks if the exact day of a specific year exists.
// this doesn't need to clamp, because this checks for an specific date.
static int scan_exact_month_day(char *input, uint8_t *output, uint8_t month, int32_t year) {
    static uint8_t days_in_month[] = { 
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31,
    };

    if (is_leap_year(year))
        days_in_month[1] = 29;
    
    if (sscanf(input, "%hhd", output) != 1)
        ERROR("Invalid day of the month.");

    if (*output > days_in_month[month])
        ERROR("This month doesn't have this amount of days.");

    return 0;
}

static int scan_month(char *input, uint8_t *output) {
    if (sscanf(input, "%hhd", output) != 1)
        ERROR("Invalid month.");

    if (*output < 1 || *output > 12)
        ERROR("Month must be between 1 and 12.");

    return 0;
}

static int scan_year(char *input, int32_t *output) {
    if (sscanf(input, "%d", output) != 1)
        ERROR("Invalid year.");

    if (*output <= 0)
        ERROR("Year must be greater than 0.");

    return 0;
}

static int scan_hour(char *input, Hour *output) {
    uint8_t hours, minutes;
    if (sscanf(input, "%hhd:%hhd", &hours, &minutes) != 2)
        ERROR("Please specify the hour correctly: 'hh:mm'.");

    *output = (Hour) {
        .hours = hours,
        .minutes = minutes,
    };

    if (!is_valid_hour(*output))
        ERROR("Please specify a valid hour.");

    return 0;
}

static int scan_week_day(char *input, uint8_t *output) {
    if (sscanf(input, "%hhd", output) != 1)
        ERROR("Invalid day of the week.");

    if (*output < 1 || *output > 7)
        ERROR("Day of the week must be between 1 and 7.");

    return 0;
}

static int parse_alarm_add(int len, char *args[]) {
    char *description = args[0]; // guaranteed
    
    if (check_description(description) != 0)
        return 1;
    
    if (len <= 1)
        ERROR("Please specify the frequency.");
    
    char *freq = args[1];
    Alarm alarm;

    if (strcasecmp(freq, "daily") == 0) {
        if (len < 3)
            ERROR("Usage: daily <hour>.");

        Hour hour;
        if (scan_hour(args[2], &hour) != 0)
            return 1;

        alarm = (Alarm) {
            .description = description,
            .type = (AlarmType) {
                .id = ALARM_DAILY,
                .alarm.daily = {
                    .hour = hour,
                },
            },
        };
    }
    
    else if (strcasecmp(freq, "weekly") == 0) {
        if (len < 4)
            ERROR("Usage: weekly <day of the week> <hour>.");

        uint8_t week_day;
        Hour hour;

        if (scan_week_day(args[2], &week_day) != 0 || scan_hour(args[3], &hour) != 0)
            return 1;

        alarm = (Alarm) {
            .description = description,
            .type = (AlarmType) {
                .id = ALARM_WEEKLY,
                .alarm.weekly = {
                    .week_day = week_day,
                    .hour = hour,
                },
            },
        };
    }

    else if (strcasecmp(freq, "monthly") == 0) {
        if (len < 4)
            ERROR("Usage: monthly <day of the month> <hour> [--clamp].");

        uint8_t month_day;
        Hour hour;
        bool clamp = false;

        if (len >= 5 && strcasecmp(args[4], "--clamp") == 0)
            clamp = true;

        if (scan_month_day(args[2], &month_day, clamp) != 0 || scan_hour(args[3], &hour) != 0)
            return 1;

        alarm = (Alarm) {
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
    }

    else if (strcasecmp(freq, "yearly") == 0) {
        if (len < 5)
            ERROR("Usage: yearly <month> <day of the month> <hour> [--clamp].");

        uint8_t month, month_day;
        Hour hour;
        bool clamp = false;

        if (len >= 6 && strcasecmp(args[5], "--clamp") == 0)
            clamp = true;
        
        if (scan_month(args[2], &month) != 0 ||
            scan_specific_month_day(args[3], &month_day, month, clamp) != 0 ||
            scan_hour(args[4], &hour) != 0)
            return 1;

        alarm = (Alarm) {
            .description = description,
            .type = (AlarmType) {
                .id = ALARM_YEARLY,
                .alarm.yearly = {
                    .month = month,
                    .month_day = month_day,
                    .hour = hour,
                    .clamp = clamp,
                },
            },
        };
    }

    else if (strcasecmp(freq, "once") == 0) {
        if (len < 6)
            ERROR("Usage: once <year> <month> <day of the month> <hour>.");
        
        uint8_t month, month_day;
        int32_t year;
        Hour hour;

        if (scan_year(args[2], &year) != 0 ||
            scan_month(args[3], &month) != 0 ||
            scan_exact_month_day(args[4], &month_day, month, year) != 0 ||
            scan_hour(args[5], &hour) != 0)
            return 1;

        if (date_has_passed(alarm.type.alarm.once, now()))
            WARN("This date has already passed, so the alarm for it won't ring.");
    
        alarm = (Alarm) {
            .description = description,
            .type = (AlarmType) {
                .id = ALARM_ONCE,
                .alarm.once = {
                    .year = year,
                    .month = month,
                    .month_day = month_day,
                    .hour = hour,
                },
            },
        };
    }

    else ERROR("Usage: add <description> <daily | weekly | monthly | yearly | once>.");

    alarm_add(alarm);
    return 0;
}

static int parse_alarm_edit(int len, char *args[]) {
    ERROR("Not implemented yet.");
}

static int parse_alarm_list(char *filter) {
    AlarmFilter filt;
    
    if (strcasecmp(filter, "daily") == 0)
        filt = ALARM_DAILY;
    else if (strcasecmp(filter, "weekly") == 0)
        filt = ALARM_WEEKLY;
    else if (strcasecmp(filter, "monthly") == 0)
        filt = ALARM_MONTHLY;
    else if (strcasecmp(filter, "yearly") == 0)
        filt = ALARM_YEARLY;
    else if (strcasecmp(filter, "once") == 0)
        filt = ALARM_ONCE;
    else
        ERROR("Invalid filter.");

    alarm_list(filt);
    return 0;
}

static int parse_remove_alarm(char *id) {
    Id out_id;
    if (sscanf(id, "%hd", &out_id) != 1)
        ERROR("Invalid ID.");

    alarm_remove(out_id);
    return 0;
}

static int check_description(char *description) {
    if (strchr(description, SEPARATOR_CHAR) != NULL)
        ERROR("The description must not contain '" SEPARATOR "\'.");

    if (strlen(description) > MAX_DESCRIPTION_LENGTH)
        ERROR("Description too long. Must have less than 1024 characters.");

    return 0;
}
