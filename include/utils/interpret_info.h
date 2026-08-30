#ifndef PESEC_INTERPRET_INFO_H
#define PESEC_INTERPRET_INFO_H
#include "typedefs.h"

typedef struct INTERPRET_INFO_STRUCT
{
    ull_t number_accurate;
    const char* filename;
} interpret_info_t;

interpret_info_t* interpret_info_get();

#endif // PESEC_INTERPRET_INFO_H