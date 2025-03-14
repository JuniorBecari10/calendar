#include <stdio.h>
#include <strings.h>

#include "lib/operations.h"

#define ERROR(message)                 \
    do {                               \
        fprintf(stderr, message "\n"); \
        return 1;                      \
    } while (0)

int main(int argc, char *argv[]) {
    switch (argc) {
        case 1: {
            print_current_month();
            return 0;
        }
        
        case 2: {
            // now, help, version, watch, default: print year
            //
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

                if (sscanf(cmd, "%d", &year) != 1)
                    ERROR("Invalid year/option.");

                if (year <= 0)
                    ERROR("Year must be greater than 0.");
            
                print_year(year);
            }

            return 0;
        }

        case 3: {
            // import, export, remove, default: print month
            // for now, these options won't be implemented

            int8_t month;
            int32_t year;

            if (sscanf(argv[1], "%hhd", &month) != 1)
                ERROR("Invalid month.");

            if (sscanf(argv[2], "%d", &year) != 1)
                ERROR("Invalid year.");

            if (month < 0 || month > 12)
                ERROR("Month must be between 1 and 12.");

            if (year <= 0)
                ERROR("Year must be greater than 0.");

            print_month(month, year);
            return 0;
        }
    }

    return 0;
}

