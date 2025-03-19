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

                        // TODO: check for errors
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

                    }

                    else if (strcasecmp(option, "monthly") == 0) {

                    }

                    else if (strcasecmp(option, "yearly") == 0) {

                    }
                    
                    else if (strcasecmp(option, "unique") == 0) {

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

