#include "include/common/panic.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

[[noreturn]] void pesec_panic(const char* format, ...)
{
    va_list args = {0};

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputc('\n', stderr);
    fflush(stderr);

    exit(EXIT_FAILURE);
}
