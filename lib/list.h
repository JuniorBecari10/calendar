#ifndef LIST_H
#define LIST_H

#include "operations.h"
#include <stddef.h>

#define INITIAL_CAP 10
#define GROW_FACTOR 2

#define MAX_LEN 10000

#define LIST_ITER(a, llist) for (Alarm *a = llist.list; (size_t) (a - llist.list) < llist.len; a++)

typedef struct {
    Alarm *list;
    size_t len;
    size_t cap;
} AlarmList;

AlarmList new_alarm_list();
void free_alarm_list(AlarmList *l);

void push_alarm(AlarmList *l, Alarm alarm);
void remove_alarm(AlarmList *l, size_t index);

#endif

