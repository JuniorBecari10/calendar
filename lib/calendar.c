#include <stdio.h>
#include "calendar.h"

static int is_leap_year(int year);
static int leap_years(int year);
static int today_of(int y, int m, int d);
static long num_days(int y, int m, int d);

void print_calendar(int y, int m) {
    const char* names_of_months[] = { NULL,
        "January", "February", "March", "April", "May", "June", "July",
        "August", "September", "October", "November", "December"
    };

    char *week = "Su Mo Tu We Th Fr Sa";
    int num_days_of_months[] = { -1, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    int week_of_top_day = num_days(y, m, 1) % 7;
    
    if (is_leap_year(y))
        num_days_of_months[2] = 29;

    printf("     %s %d\n%s\n", names_of_months[m], y, week);

    for (int i = 0; i < week_of_top_day; i++)
        printf("   ");

    for (int i = week_of_top_day, day = 1; day <= num_days_of_months[m]; i++, day++) {
        printf("%2d ", day);

        if (i % 7 == 6)
            printf("\n");
    }

    printf("\n");
}

static int is_leap_year(int y) {
    return (y % 400 == 0)
        || ((y % 4 == 0)
        && (y % 100 != 0));
}

static int leap_years(int y) {
    return y / 4 - y / 100 + y / 400;
}

static int today_of(int y, int m, int d) {
    static int day_of_months[] = { -1, 0, 31, 59, 120, 151, 181, 212, 243, 273, 304, 334 };
    return day_of_months[m] + d + ((m > 2 && is_leap_year(y)) ? 1 : 0);
}

static long num_days(int y, int m, int d) {
    int last_year = y - 1;
    return 365L * last_year + leap_years(last_year) + today_of(y, m, d);
}

