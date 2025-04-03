#include <stdio.h>

#include "operations.h"
#include "calendar.h"
#include "print.h"
#include "file.h"
#include "list.h"
#include "util.h"

const char *VERSION = "v0.1 Alpha";

void print_current_month() {
    Date date_now = now();
    print_calendar(date_now.year, date_now.month);
}

void print_year(int year) {
    printf("     Year of %d\n\n", year);

    for (int m = 1; m <= 12; m++) {
        print_calendar(year, m);
        printf("\n");
    }
}

void print_month(int month, int year) {
    print_calendar(year, month);
}

void print_now() {
    DateComplete now = now_complete();

    print_week_day(now.week_day);
    printf(", ");

    pretty_print_month(now.month);
    printf(" %hhd, %d (", now.month_day, now.year);

    print_date_iso(now.year, now.month, now.month_day);
    printf(") ");

    print_hour_seconds(now.hour);
    printf("\n");
}

void help() {
    printf(
        "Calendar CLI\n\n"

        "Commands:\n"
        "    (no args)             - show current month\n"
        "    <year>                - print all months of the specified year\n"
        "    <month> <year>        - print the specified month of the specified year.\n\n"

        "    now                   - show the current day of the week, date and time\n"
        "    help                  - show this help message\n"
        "    version               - show version information\n"
        "    watch                 - watch mode; required for the alarms to ring\n\n"

        "    import <file> [-y]    - import calendar from the specified file. this operation discards the current calendar file.\n"
        "    export <file>         - export calendar to the specified file.\n\n"

        "    alarm                 - manage alarms\n"
        "        add <description> - add a new alarm\n"
        "            | daily   <hour - hh:mm>\n"
        "            | weekly  <day of the week - 1-7> <hour - hh:mm>\n"
        "            | monthly <day of the month> <hour - hh:mm> [--clamp]\n"
        "            | yearly  <month - 1-12> <day of the month> <hour - hh:mm> [--clamp]\n"
        "            | once    <year> <month - 1-12> <day of the month> <hour - hh:mm>\n\n"

        "        edit <id> <description> - edits an existing alarm\n"
        "            | daily   <hour - hh:mm>\n"
        "            | weekly  <day of the week - 1-7> <hour - hh:mm>\n"
        "            | monthly <day of the month> <hour - hh:mm> [--clamp]\n"
        "            | yearly  <month - 1-12> <day of the month> <hour - hh:mm> [--clamp]\n"
        "            | once    <year> <month - 1-12> <day of the month> <hour - hh:mm>\n\n"

        "       list [\n"
        "            | daily\n"
        "            | weekly\n"
        "            | monthly\n"
        "            | yearly\n"
        "            | once\n"
        "            ]             - lists all the alarms acording to the specified filters\n"
        "       remove <id>        - removes the specified alarm\n"
    );
}

void version() {
    printf("Calendar %s.\n", VERSION);
}

void watch() {
    PERROR("Not implemented yet.");
}

void import_calendar(char *file) {
    PERROR("Not implemented yet.");
}

void export_calendar(char *file) {
    PERROR("Not implemented yet.");
}

void alarm_add(Alarm alarm) {
    AlarmList list;
    if (!parse_file(&list)) return;

    // add an id
    alarm.id = random_uint32();
    push(&list, alarm);

    if (!write_to_file(list)) return;
    free_alarm_list(&list);

    print_alarm(alarm);
    printf("Alarm added successfully.\n");
}

void alarm_edit(Id id, Alarm alarm) {
    PERROR("Not implemented yet.");
}

void alarm_list(AlarmFilter filter) {
    PERROR("Not implemented yet.");
}

void alarm_remove(Id id) {
    PERROR("Not implemented yet.");
}

