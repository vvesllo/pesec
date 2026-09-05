#include "include/utils/throw.h"

#include "include/context.h"
#include "include/utils/interpret_info.h"
#include "include/utils/memory.h"


extern context_t* global_current_context;

void throw_cleanup()
{
    if (global_current_context)
        context_free(global_current_context);

    interpret_info_t* interpret_info = interpret_info_get();
    if (interpret_info)
        free(interpret_info);

    memory_free(memory_get());
}