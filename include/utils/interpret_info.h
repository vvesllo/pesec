#ifndef PESEC_INTERPRET_INFO_H
#define PESEC_INTERPRET_INFO_H
#include "typedefs.h"
#include "include/vector_value.h"

typedef struct INTERPRET_INFO_STRUCT
{
    ull_t number_accuracy;
    const char* filename;
    vector_value_t* args;
} interpret_info_t;

interpret_info_t* interpret_info_get();

#endif // PESEC_INTERPRET_INFO_H