#include "list.h"
#include "util.h"

#include <string.h>
#include <stdio.h>

#define MAX_LINE 2048

static Alarm parse_line(char *line);

AlarmList parse_file() {
    // for now
    FILE *file = fopen(".alarms", "r");
    AlarmList list = new_alarm_list();
    
    if (!file) {
        // the file doesn't exist yet or some another error.
        // either case, we'll return an empty list.
        return list;
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file)) {
        if (strchr(line, '\n') == NULL) {
            PERROR("Parse error: line too long to parse.\n");
            free_alarm_list(&list);

            // TODO: return an error as well
            return list;
        }

        line[strcspn(line, "\n")] = '\0'; // properly terminate the string.
        push(&list, parse_line(line));
    }

    fclose(file);
    return list;
}

static Alarm parse_line(char *line) {

}

