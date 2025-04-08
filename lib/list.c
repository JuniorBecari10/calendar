#include <stdlib.h>
#include <stddef.h>

#include "list.h"
#include "operations.h"

AlarmList new_alarm_list() {
    return (AlarmList) {
        .list = NULL,
        .len = 0,
        .cap = 0,
    };
}

void free_alarm_list(AlarmList *l) {
    for (Alarm *a = l->list; (size_t) (a - l->list) < l->len; a++) {
        free(a->description);
    }

    free(l->list);
    *l = new_alarm_list();
}

void push_alarm(AlarmList *l, Alarm alarm) {
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

void remove_alarm(AlarmList *l, size_t index) {
    if (index >= l->len)
        return; // Invalid index, do nothing
   
    free(l->list[index].description);
    for (size_t i = index; i < l->len - 1; i++)
        l->list[i] = l->list[i + 1];
    
    l->len--;
    
    if (l->len == 0)
        free_alarm_list(l);
}

