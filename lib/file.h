#ifndef FILE_H
#define FILE_H

#include <stdbool.h>
#include "list.h"

bool parse_file(AlarmList *out_list);
bool write_to_file(AlarmList list);

#endif
