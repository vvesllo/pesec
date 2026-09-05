#include "include/utils/interpret_info.h"

#include <stdlib.h>

interpret_info_t* interpret_info_get()
{
    static interpret_info_t* interpret_info = nullptr;
    if (!interpret_info)
    {
        interpret_info = (interpret_info_t*)malloc(sizeof(interpret_info_t));
        interpret_info->number_accuracy=16;
        interpret_info->filename=nullptr;
    }

    return interpret_info;
}
