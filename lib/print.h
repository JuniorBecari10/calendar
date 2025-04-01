#ifndef PRINT_H
#define PRINT_H

#include "operations.h"

void print_alarm(Alarm alarm);
void print_hour(Hour hour);
void print_hour_seconds(HourSeconds hour);
void print_week_day(uint8_t day);
void pretty_print_month(uint8_t month);

void print_date_iso(uint32_t year, uint8_t month, uint8_t day);

#endif

