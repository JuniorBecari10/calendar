#ifndef UTIL_H
#define UTIL_H

#define ERROR(message)                 \
    do {                               \
        fprintf(stderr, message "\n"); \
        return 1;                      \
    } while (0)

#define PERROR(message)                 \
    do {                               \
        fprintf(stderr, message "\n"); \
    } while (0)

#endif
