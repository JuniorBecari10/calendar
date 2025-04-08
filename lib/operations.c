#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#include "operations.h"
#include "calendar.h"
#include "print.h"
#include "file.h"
#include "list.h"
#include "util.h"

volatile bool requested_close = false;
const char *VERSION = "v0.1 Alpha";

static void handle_close();

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
        "Calendar\n\n"

        "Commands:\n"
        "    (no args)             - show current month\n"
        "    <year>                - print all months of the specified year\n"
        "    <month> <year>        - print the specified month of the specified year.\n\n"

        "    now                   - show the current day of the week, date and time\n"
        "    help                  - show this help message\n"
        "    version               - show version information\n"
        "    watch                 - watch mode; required for the alarms to ring\n\n"

        "    import <file> [-y]    - import calendar from the specified file.\n"
        "    export <file>         - export calendar to the specified file.\n\n"

        "    alarm                 - manage alarms\n"
        "        add <description> - add a new alarm\n"
        "            | daily   <hour - hh:mm>\n"
        "            | weekly  <day of the week - 1-7> <hour - hh:mm>\n"
        "            | monthly <day of the month> <hour - hh:mm> [--clamp]\n"
        "            | yearly  <month - 1-12> <day of the month> <hour - hh:mm> [--clamp]\n"
        "            | once    <year> <month - 1-12> <day of the month> <hour - hh:mm>\n\n"

        "        edit <id> <description> - edit an existing alarm\n"
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
        "            ]             - list all the alarms acording to the specified filters\n\n"

        "       remove <id> [-y]   - remove the specified alarm\n"
        "       clear  [-y]        - clear all alarms\n"
    );
}

void version() {
    printf("Calendar %s.\n", VERSION);
}

void watch() {
    signal(SIGINT, handle_close);

    AlarmList list;
    if (!parse_file(&list)) return;

    for (;;) {
        AlarmList ring_today = get_alarms_to_ring_today(list);
        AlarmList ring_now = get_alarms_to_ring_now(ring_today);
        
        CLEAR();

        printf("Calendar - Watch Mode\n");
        print_now();
        printf("Press Ctrl-C to exit.\n\n");

        if (ring_now.len > 0) {
            printf("Ringing now:\n");

            LIST_ITER(a, ring_now) {
                print_alarm(*a);
            }

            printf("\a\n");
        }

        if (list.len == 0)
            printf("There are no alarms.\n");
        else if (ring_today.len == 0)
            printf("There are no alarms to ring today.\n");
        else {
            printf("Alarms to ring today:\n");
            
            LIST_ITER(a, ring_today) {
                print_alarm(*a);
            }
        }

        if (requested_close) {
            free_alarm_list(&list);
            free_alarm_list(&ring_today);
            free_alarm_list(&ring_now);

            return;
        }

        sleep(1);
        free_alarm_list(&ring_today);
        free_alarm_list(&ring_now);
    }
}

void import_calendar(char *file, bool yes) {
    if (!yes && !ask_for_confirmation("Do you really want to import this calendar?"))
        return;

    FILE *new_calendar = fopen(file, "r");

    if (new_calendar == NULL)
        ERRORR("Cannot read the specified file.");

    AlarmList out = new_alarm_list();
    if (!parse_file(&out)) {
        PERROR("The specified file is not a valid calendar file.\nThe original file was not modified.");

        fclose(new_calendar);
        free_alarm_list(&out);
        return;
    }

    // copy the contents
    FILE *original = get_file_writer();

    char c;
    while ((c = fgetc(new_calendar)) != EOF)
        fputc(c, original);

    fclose(original);
    fclose(new_calendar);
    free_alarm_list(&out);

    printf("Calendar successfully imported.\n");
}

void export_calendar(char *file) {
    FILE *export = fopen(file, "w");

    if (export == NULL)
        ERRORR("Cannot write to the specified file.");

    FILE *original = get_file_reader();

    char c;
    while ((c = fgetc(original)) != EOF)
        fputc(c, export);

    fclose(original);
    fclose(export);

    printf("Calendar successfully exported.\n");
}

// TODO: don't allow duplicates (alarms with the same frequency, date and time)
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
            free(a->description);

            *a = alarm;

            print_alarm(alarm);
            printf("Alarm edited successfully.\n");

            write_to_file(list); // error handling is redundant here because either way we will free the list and return
            free_alarm_list(&list);
            return;
        }
    }

    PERROR("There's no alarm with this ID.");
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

void alarm_remove(Id id, bool yes) {
    if (!yes && !ask_for_confirmation("Do you really want to remove this alarm?"))
        return;
    
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
    write_to_file(list);
    free_alarm_list(&list);

    printf("Alarm removed.\n");
}

void alarm_clear(bool yes) {
    if (!yes && !ask_for_confirmation("Do you really want to clear your alarms?"))
        return;

    char *filename = get_file_path();

    remove(filename);
    free(filename);

    printf("Cleared all alarms.\n");
}

static void handle_close() {
    requested_close = true;
}

