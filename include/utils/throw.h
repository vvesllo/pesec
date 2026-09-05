#ifndef PESEC_THROW_H
#define PESEC_THROW_H

#include <stdio.h>
#include <stdlib.h>

#define THROW(...) \
    do { \
        fprintf(stderr, __VA_ARGS__); \
        throw_cleanup(); \
        exit(EXIT_FAILURE);\
    } while (0)

void throw_cleanup();


#endif // PESEC_THROW_H
