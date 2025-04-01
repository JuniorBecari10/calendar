#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <stdbool.h>
#include <inttypes.h>

typedef struct {
    uint8_t hours;
    uint8_t minutes;
} Hour;

typedef struct {
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
} HourSeconds;

typedef struct {
    uint32_t year;
    uint8_t month;
    uint8_t month_day;
    uint8_t week_day;

    HourSeconds hour;
} DateComplete;

typedef enum {
    ALARM_DAILY,
    ALARM_WEEKLY,
    ALARM_MONTHLY,
    ALARM_YEARLY,
    ALARM_ONCE,
} AlarmTypeId;

typedef struct {
    AlarmTypeId id;
    
    union {
        struct AlarmDaily {
            Hour hour;
        } daily;

        struct AlarmWeekly {
            uint8_t week_day;
            Hour hour;
        } weekly;

        struct AlarmMonthly {
            uint8_t month_day;
            Hour hour;
            bool clamp; // clamps the day if the current month doesn't have enough days.
                        // (ex.: month_day = 31. February doesn't have 31 days, so the alarm will ring on the last day, which
                        // can be either 28 or 29). default is false.
        } monthly;

        struct AlarmYearly {
            uint8_t month;
            uint8_t month_day;

            Hour hour;
            bool clamp; // because of february, which in leap years it has 29 days
        } yearly;

        struct AlarmOnce {
            uint32_t year;
            uint8_t month;
            uint8_t month_day;

            Hour hour;
        } once;
    } alarm;
} AlarmType;

typedef struct {
    char *description;
    AlarmType type;
    uint32_t id;
} Alarm;

typedef uint64_t Id;
typedef AlarmTypeId AlarmFilter;
typedef struct AlarmOnce Date;

/* 
 * Args:
 *
 * 0 -> show current month ok
 * 1 (year) -> show entire year ok
 * 2 (month year) -> show specified month ok
 *
 * now -> show current date and time ok
 * help -> show help text ok*
 * version -> show version and details ok
 * watch -> watch mode, where the app can work and ring your alarms ok*
 *
 * import <file> [-y] -> set the global '.calendar' file to the specified one if it's valid, '-y' flag to not ask for confirmation ok*
 * export <file> -> export the current '.calendar' file to the specified file ok*
 * 
 * alarm:
 *     add <description> (daily <hh:mm> | weekly <week-day> <hh:mm>
 *         | monthly <day*> <hh:mm> | yearly <day> <month> <hh:mm> | once <day> <month> <year> <hh:mm>) -> add an alarm for the specified interval
 *     *: that is available on all months
 *
 *     edit <id> /same as 'add'/
 *
 *     list [daily | weekly | monthly | once] -> lists all the alarms acording to the specified filters
 *     remove <id> -> removes the specified alarm
 */

void print_current_month();
void print_year(int year);
void print_month(int month, int year);

void print_now();

void help();
void version();
void watch();

void import_calendar(char *file);
void export_calendar(char *file);

void alarm_add(Alarm alarm);
void alarm_edit(Id id, Alarm alarm);

void alarm_list(AlarmFilter filter);
void alarm_remove(Id id);

#endif
