#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <inttypes.h>

typedef struct {
    uint8_t hours;
    uint8_t minutes;
} Hour;

typedef enum {
    ALARM_DAILY,
    ALARM_WEEKLY,
    ALARM_MONTHLY,
    ALARM_UNIQUE,
} AlarmTypeId;

typedef struct {
    AlarmTypeId id;
    
    union {
        struct {
            Hour hour;            
        } daily;

        struct {
            uint8_t week_day;
            Hour hour;
        } weekly;

        struct {
            uint8_t month_day;
            Hour hour;
        } monthly;

        struct {
            uint8_t month_day;
            uint8_t month;
            uint32_t year;

            Hour hour;
        } unique;
    } alarm;
} AlarmType;

typedef uint64_t Id;
typedef AlarmTypeId AlarmFilter;

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
 * import <file> -> set the global '.calendar' file to the specified one if it's valid ok*
 * export <file> -> export the current '.calendar' file to the specified file ok*
 * 
 * alarm:
 *     add <description> (daily <hh:mm> | weekly <week-day> <hh:mm>
 *         | monthly <day*> <hh:mm> | unique <day> <month> <year> <hh:mm>) -> add an alarm for the specified interval
 *     *: that is available on all months
 *
 *     edit <id> /same as 'add'/
 *
 *     list [daily | weekly | monthly | unique] -> lists all the alarms acording to the specified filters
 *     remove <id> -> removes the specified alarm
 */

void print_current_month();
void print_year(int year);
void print_month(int month, int year);

void print_now();

void help();
void version();
void watch();

void import_file(char *file);
void export_file(char *file);

void alarm_add(char *description, AlarmType type);
void alarm_edit(Id id, char *description, AlarmType type);

void alarm_list(AlarmFilter filter);
void alarm_remove(Id id);

#endif
