#include <stdio.h>
#include <strings.h>

#include "lib/operations.h"

int main(int argc, char *argv[]) {
    switch (argc) {
        case 1: {
            print_current_month();
            return 0;
        }
        
        case 2: {
            // now, help, version, watch, default: print year
            char *cmd = argv[1];

            if (strcasecmp(cmd, "now") == 0)
                print_now();
            else if (strcasecmp(cmd, "help") == 0)
                help();
            else if (strcasecmp(cmd, "version") == 0)
                version();
            else if (strcasecmp(cmd, "watch") == 0)
                watch();
            else {
                int32_t year;

                if (sscanf(cmd, "%d", &year) != 1) {
                    fprintf(stderr, "Invalid year.\n");
                    return 1;
                }

                print_year(year);
            }

            return 0;
        }

        case 3: {
            // import, export, remove, default: print month
            // for now, these options won't be implemented

            uint8_t month;
            uint32_t year;

            if (sscanf(argv[1], "%hhu", &month) != 1 || sscanf(argv[2], "%d", &year) != 1) {
                fprintf(stderr, "Incorrect formatting for month and year.\n");
                return 1;
            }

            print_month(month, year);
            return 0;
        }
    }

    return 0;
}

