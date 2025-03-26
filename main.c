#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <strings.h>

#include "lib/operations.h"
#include "lib/util.h"

int scan_month(char *input, int8_t *output);
int scan_year(char *input, int32_t *output);
int scan_hour(char *input, Hour *output);

int parse_alarm_add(int len, char *args[]);
int parse_alarm_edit(int len, char *args[]);
int parse_alarm_list(char *filter);
int remove_alarm(char *id);

int main(int argc, char *argv[]) {
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
            parse_alarm_list(argv[1]);
        
        else if (strcasecmp(suboption, "remove") == 0 && argc == 1)
            remove_alarm(argv[1]);
        
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
        int8_t month;
        int32_t year;

        if (scan_month(argv[1], &month) != 0 || scan_year(argv[2], &year) != 0)
            return 1;

        print_month(month, year);
    }

    else
        ERROR("Invalid option.");
    
    return 0;
}

int scan_month(char *input, int8_t *output) {
    if (sscanf(input, "%hhd", output) != 1)
        ERROR("Invalid month.");

    if (*output < 1 || *output > 12)
        ERROR("Month must be between 1 and 12.");

    return 0;
}

int scan_year(char *input, int32_t *output) {
    if (sscanf(input, "%d", output) != 1)
        ERROR("Invalid year.");

    if (*output <= 0)
        ERROR("Year must be greater than 0.");

    return 0;
}

int scan_hour(char *input, Hour *output) {

}

int parse_alarm_add(int len, char *args[]) {
    char *description = args[0]; // guaranteed

    if (strchr(description, SEPARATOR_CHAR) != NULL)
        ERROR("The description must not contain '" SEPARATOR "\'.");
    
    if (len == 1)
        ERROR("Please specify the frequency.");
    
    char *freq = args[1];
    Alarm alarm;

    if (strcasecmp(freq, "daily") == 0) {
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

    }

    alarm_add(alarm);
    return 0;
}

int parse_alarm_edit(int len, char *args[]) {

}

int parse_alarm_list(char *filter) {

}

int remove_alarm(char *id) {

}

