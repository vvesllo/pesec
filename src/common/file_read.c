#include "include/common/file_read.h"

#include <stdio.h>

#include "include/common/alloc.h"
#include "include/common/panic.h"

pesec_string_view_t pesec_file_read(const char* filename)
{
    FILE* file = fopen(filename, "rb");
    if (!file)
    {
        pesec_panic("file_read: can't read file `%s`", filename);
    }

    if (fseek(file, 0, SEEK_END) != 0)
    {
        fclose(file);
        pesec_panic("file_read: failed to read file `%s`", filename);
    }

    const size_t file_size = ftell(file);
    rewind(file);

    const auto buffer = (char*)pesec_malloc(sizeof(char) * (file_size + 1));
    fread(buffer, sizeof(char), file_size, file);
    buffer[file_size] = '\0';

    fclose(file);

    return (pesec_string_view_t){
        .data = buffer,
        .size = file_size
    };
}
