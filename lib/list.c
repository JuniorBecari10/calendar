#include "list.h"
#include "operations.h"

#include <stdlib.h>
#include <stddef.h>

AlarmList new_alarm_list() {
    return (AlarmList) {
        .list = NULL,
        .len = 0,
        .cap = 0,
    };
}

void free_alarm_list(AlarmList *l) {
    free(l->list);
    *l = new_alarm_list();
}

void push(AlarmList *l, Alarm alarm) {
    if (l->list == NULL) {
        l->list = (Alarm *) malloc(INITIAL_CAP * sizeof(Alarm));
        l->cap = INITIAL_CAP;
    }

    else if (l->cap < l->len + 1) {
        l->list = realloc(l->list, l->cap * 2);
        l->cap *= 2;
    }

    l->list[l->len++] = alarm;
}

