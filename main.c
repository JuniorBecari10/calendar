#include <stdio.h>
#include <strings.h>
#include <time.h>

#include "calendar.h"

const char *VERSION = "Alpha v0.1";

static void parse_operation(int argc, char *argv[]);
static void print_calendar_current_month();

int main(int argc, char **argv) {
    switch (argc) {
        // No args: Show current month calendar
        case 1: {
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);

            calendar(tm.tm_year + 1900, tm.tm_mon + 1);
            break;
        }

        // One arg: options
        case 2: {
            if (strcasecmp(argv[1], "version") == 0)
                printf("%s\n", VERSION);
            else
                fprintf(stderr, "Usage: %s version\n", argv[0]);

            break;
        }

        // Two args (year month): Show calendar of the specified year and month
        case 3: {
            int year, month;

            if (!sscanf(argv[1], "%d", &year) || !sscanf(argv[2], "%d", &month)) {
                fprintf(stderr, "Usage: %s year month\n", argv[0]);
                return 1;
            }

            calendar(year, month);
            break;
        }

        default: {
            if (strcasecmp(argv[1], "schedule")) {
                
            }

            fprintf(stderr, "Incorrect usage.\n");
            return 1;
        }
    }

    return 0;
}

static void parse_operation(int argc, char *argv[]) {
    if (argc == 1) {
        print_calendar_current_month();
        return;
    }

    char *op = argv[1];
    if (strcasecmp(op, ""))
}

static void print_calendar_current_month() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    print_calendar(tm.tm_year + 1900, tm.tm_mon + 1);
}

