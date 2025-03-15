#include <stdio.h>
#include <time.h>

#include "operations.h"
#include "calendar.h"

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
    const char *days_of_the_week[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    
    printf("%s, %d/%02d/%02d %02d:%02d:%02d\n", days_of_the_week[tm.tm_wday], tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
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

void import_file(char *file) {
    fprintf(stderr, "Not implemented yet.\n");
}

void export_file(char *file) {
    fprintf(stderr, "Not implemented yet.\n");
}

void alarm_add(char *description, AlarmType type) {
    fprintf(stderr, "Not implemented yet.\n");
}

void alarm_edit(Id id, char *description, AlarmType type) {
    fprintf(stderr, "Not implemented yet.\n");
}

void alarm_list(AlarmFilter filter) {
    fprintf(stderr, "Not implemented yet.\n");
}

void alarm_remove(Id id) {
    fprintf(stderr, "Not implemented yet.\n");
}

