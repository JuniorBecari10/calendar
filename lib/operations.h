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

typedef uint16_t Id;
typedef AlarmTypeId AlarmFilter;
typedef struct AlarmOnce Date;

typedef struct {
    uint32_t year;
    uint8_t month;
    uint8_t month_day;
    uint8_t week_day;

    HourSeconds hour;
} DateComplete;

typedef struct {
    char *description;
    AlarmType type;
    Id id;
} Alarm;

/* 
 * args:
 *
 * 0 -> show current month
 * 1 (year) -> show entire year
 * 2 (month year) -> show specified month
 *
 * now -> show current date and time
 * help -> show help text
 * version -> show version and details
 * watch -> watch mode, where the app can work and ring your alarms
 *
 * import <file> [-y] -> set the global '.when' file to the specified one if it's valid, '-y' flag to not ask for confirmation
 * export <file> -> export the current '.when' file to the specified file
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

void import_alarms(char *file, bool yes);
void export_alarms(char *file);

void alarm_add(Alarm alarm, bool yes);
void alarm_edit(Id id, Alarm alarm);

void alarm_list_all();
void alarm_list(AlarmFilter filter);
void alarm_remove(Id id, bool yes);
void alarm_clear(bool yes);

#endif

