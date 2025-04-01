#include <stdio.h>
#include <time.h>

#include "operations.h"
#include "calendar.h"
#include "print.h"
#include "file.h"
#include "list.h"
#include "util.h"

const char *VERSION = "v0.1 Alpha";

void print_current_month() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    print_calendar(tm.tm_year + 1900, tm.tm_mon + 1);
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
    fprintf(stderr, "Not implemented yet.\n");
}

void version() {
    printf("Calendar %s\n", VERSION);
}

void watch() {
    fprintf(stderr, "Not implemented yet.\n");
}

void import_calendar(char *file) {
    fprintf(stderr, "Not implemented yet.\n");
}

void export_calendar(char *file) {
    fprintf(stderr, "Not implemented yet.\n");
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
    fprintf(stderr, "Not implemented yet.\n");
}

void alarm_list(AlarmFilter filter) {
    fprintf(stderr, "Not implemented yet.\n");
}

void alarm_remove(Id id) {
    fprintf(stderr, "Not implemented yet.\n");
}

