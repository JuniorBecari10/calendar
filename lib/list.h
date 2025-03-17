#ifndef LIST_H
#define LIST_H

#include "operations.h"
#include <stddef.h>

#define INITIAL_CAP 10
#define GROW_FACTOR 2

typedef struct {
    Alarm *list;
    size_t len;
    size_t cap;
} AlarmList;

AlarmList new_alarm_list();
void free_alarm_list(AlarmList *l);

void push(AlarmList *l, Alarm alarm);

#endif

