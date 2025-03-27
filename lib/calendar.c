#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>

#include "calendar.h"

static int32_t leap_years(int32_t year);
static int32_t today_of(int32_t year, int32_t month, int32_t day);
static uint64_t num_days(int32_t year, int32_t month, int32_t day);

void print_calendar(int32_t year, int32_t month) {
    if (month < 1 || month > 12 || year <= 0)
        return;

    const char* names_of_months[] = { NULL,
        "January", "February", "March", "April", "May", "June", "July",
        "August", "September", "October", "November", "December"
    };

    const char *week = "Su Mo Tu We Th Fr Sa";
    int32_t num_days_of_months[] = { -1, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    int32_t offsets[] = { -1, 0, 0, 0, 5, 4, 5, 4, 4, 5, 4, 5, 6 };

    int32_t week_of_top_day = (num_days(year, month, 1) + offsets[month]) % 7;

    if (is_leap_year(year))
        num_days_of_months[2] = 29;

    printf("     %s %" PRId32 "\n%s\n", names_of_months[month], year, week);

    for (int32_t i = 0; i < week_of_top_day; i++)
        printf("   ");

    for (int32_t i = week_of_top_day, day = 1; day <= num_days_of_months[month]; i++, day++) {
        printf("%2" PRId32 " ", day);

        if (i % 7 == 6 && day < num_days_of_months[month])
            printf("\n");
    }

    printf("\n");
}

bool is_leap_year(int32_t year) {
    return (year % 400 == 0)
        || ((year % 4 == 0)
        && (year % 100 != 0));
}

static int32_t leap_years(int32_t year) {
    return year / 4 - year / 100 + year / 400;
}

static int32_t today_of(int32_t year, int32_t month, int32_t day) {
    static int32_t day_of_months[] = { -1, 0, 31, 59, 120, 151, 181, 212, 243, 273, 304, 334 };
    return day_of_months[month] + day + ((month > 2 && is_leap_year(year)) ? 1 : 0);
}

static uint64_t num_days(int32_t year, int32_t month, int32_t day) {
    int32_t last_year = year - 1;
    return 365ULL * last_year + (uint64_t)leap_years(last_year) + (uint64_t)today_of(year, month, day);
}
