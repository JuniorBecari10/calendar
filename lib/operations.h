#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <inttypes.h>

typedef enum {
    ALARM_DAILY,
    ALARM_WEEKLY,
    ALARM_MONTHLY,
    ALARM_UNIQUE,
} AlarmTypeId;

typedef struct {

} AlarmType;

typedef uint64_t Id;
typedef AlarmTypeId Filter;

/* 
 * Args:
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
 * import <file> -> set the global '.calendar' file to the specified one if it's valid
 * export <file> -> export the current '.calendar' file to the specified file
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

void alarm_list(Filter filter);
void alarm_remove(Id id);

#endif
