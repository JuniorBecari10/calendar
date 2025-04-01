#include <stdio.h>

#include "print.h"
#include "operations.h"

static void print_hour(Hour hour);
static void print_week_day(uint8_t day);
static void pretty_print_month(uint8_t month);

static void print_date_iso(uint32_t year, uint8_t month, uint8_t day);
static void print_date_iso_no_year(uint8_t month, uint8_t day);

// desc | id | <details>
void print_alarm(Alarm alarm) {
    printf("%s | id: %d | ", alarm.description, alarm.id);

    switch (alarm.type.id) {
        case ALARM_DAILY: {
            struct AlarmDaily daily = alarm.type.alarm.daily;
            printf("Daily - ");
            print_hour(daily.hour);

            printf("\n");
            break;
        }

        case ALARM_WEEKLY: {
            struct AlarmWeekly weekly = alarm.type.alarm.weekly;
            printf("Weekly - All ");
            print_week_day(weekly.week_day);

            printf("s - ");
            print_hour(weekly.hour);

            printf("\n");
            break;
        }

        case ALARM_MONTHLY: {
            struct AlarmMonthly monthly = alarm.type.alarm.monthly;
            printf("Monthly - %hhd - ", monthly.month_day);
            print_hour(monthly.hour);

            if (monthly.clamp)
                printf(" (clamp)");

            printf("\n");
            break;
        }

        case ALARM_YEARLY: {
            struct AlarmYearly yearly = alarm.type.alarm.yearly;
            printf("Yearly - ");

            pretty_print_month(yearly.month);

            printf(" %hhd ", yearly.month_day);

            printf("(");
            print_date_iso_no_year(yearly.month, yearly.month_day);
            printf(") - ");

            print_hour(yearly.hour);

            if (yearly.clamp)
                printf(" (clamp)");

            printf("\n");
            break;
        }

        case ALARM_ONCE: {
            struct AlarmOnce once = alarm.type.alarm.once;
            printf("Once - ");
            
            pretty_print_month(once.month);

            printf(" %hhd, %d ", once.month_day, once.year);
            
            printf("(");
            print_date_iso(once.year, once.month, once.month_day);
            printf(") - ");
            
            print_hour(once.hour);
            
            printf("\n");
            break;
        }
    }
}

static void print_hour(Hour hour) {
    printf("%02hhd:%02hhd", hour.hours, hour.minutes);
}

static void print_week_day(uint8_t day) {
    #define X(a) printf(a); break

    switch (day) {
        case 1: X("Sunday");
        case 2: X("Monday");
        case 3: X("Tuesday");
        case 4: X("Wednesday");
        case 5: X("Thursday");
        case 6: X("Friday");
        case 7: X("Saturday");
    }

    #undef X
}

static void pretty_print_month(uint8_t month) {
    #define X(a) printf(a); break

    switch (month) {
        case 1:  X("January");
        case 2:  X("February");
        case 3:  X("March");
        case 4:  X("April");
        case 5:  X("May");
        case 6:  X("June");
        case 7:  X("July");
        case 8:  X("August");
        case 9:  X("September");
        case 10: X("October");
        case 11: X("November");
        case 12: X("December");
    }

    #undef X
}

static void print_date_iso(uint32_t year, uint8_t month, uint8_t day) {
    printf("%d-%02d-%02d", year, month, day);
}

static void print_date_iso_no_year(uint8_t month, uint8_t day) {
    printf("%02d-%02d", month, day);
}
