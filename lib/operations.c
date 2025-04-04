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

// TODO: add 'yes' flag
void import_calendar(char *file) {
    PERROR("Not implemented yet.");
}

void export_calendar(char *file) {
    PERROR("Not implemented yet.");
}

void alarm_add(Alarm alarm) {
    AlarmList list;
    if (!parse_file(&list)) return;

    if (list.len == MAX_LEN) {
        PERROR("Cannot add more alarms; the limit was exceeded.");
        
        free_alarm_list(&list);
        return;
    }

    // add an unique id
    alarm.id = random_unique_id(&list);
    push_alarm(&list, alarm);

    if (!write_to_file(list)) {
        free_alarm_list(&list);
        return;
    }

    print_alarm(alarm);
    printf("Alarm added successfully.\n");
    
    free_alarm_list(&list);
}

void alarm_edit(Id id, Alarm alarm) {
    AlarmList list;
    if (!parse_file(&list)) return;
    
    LIST_ITER(a, list) {
        if (a->id == id) {
            alarm.id = a->id;
            *a = alarm;

            print_alarm(alarm);
            printf("Alarm edited successfully.\n");

            write_to_file(list); // error handling is redundant here because either way we will free the list and return
            free_alarm_list(&list);
            return;
        }
    }

    PERROR("There is no alarm with this ID.");
    free_alarm_list(&list);
}

void alarm_list_all() {
    AlarmList list;
    if (!parse_file(&list)) return;
    
    if (list.len == 0) {
        printf("There are no alarms.\n");
        free_alarm_list(&list);
        
        return;
    }
    
    LIST_ITER(a, list) 
        print_alarm(*a);

    free_alarm_list(&list);
}
void alarm_list(AlarmFilter filter) {
    AlarmList list;
    if (!parse_file(&list)) return;

    if (list.len == 0) {
        printf("There are no alarms.\n");
        free_alarm_list(&list);
        
        return;
    }

    bool printed = false;
    LIST_ITER(a, list) {
        if (a->type.id == filter) {
            print_alarm(*a);
            printed = true;
        }
    }

    if (!printed)
        printf("There are no alarms that satisfy this filter.\n");

    free_alarm_list(&list);
}

void alarm_remove(Id id) {
    AlarmList list;
    if (!parse_file(&list)) return;
    
    size_t index = 0;
    bool found = false;

    LIST_ITER(a, list) {
        if (a->id == id) {
            found = true;
            break;
        }

        index++;
    }

    if (!found) {
        PERROR("There's no alarm with this ID.");
        
        free_alarm_list(&list);
        return;
    }

    remove_alarm(&list, index);
    
    if (!write_to_file(list)) {
        free_alarm_list(&list);
        return;
    }
    
    free_alarm_list(&list);
}

