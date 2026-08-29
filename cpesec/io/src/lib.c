#include "../include/lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "../../../include/function_value.h"
#include "../../../include/string_value.h"
#include "../../../include/value.h"
#include "../../../include/number_value.h"
#include "include/utils/throw.h"

#define REG_FUNC(function_name, ...) \
context_push( \
context, \
string_view_from(#function_name), \
value_new_function( \
function_value_new( \
parameter_new_from_cstr((const char *[]){__VA_ARGS__, nullptr}), \
(function_value_value_t){ \
.as_c_function = function_name \
}, \
FUNCTION_VALUE_TYPE_C_FUNCTION, \
context \
) \
), \
true \
)


static char *value_to_string(const value_t value)
{
    char *buffer = NULL;
    size_t size = 0;
    FILE *stream = open_memstream(&buffer, &size);
    if (!stream)
    {
        THROW("Failed to open memory stream");
    }
    value_print(stream, value);
    fflush(stream);
    fclose(stream);
    return buffer; // вызывающий должен освободить через free()
}

// Вспомогательная функция для извлечения целого числа из number_value_t
static long long number_value_to_int(const number_value_t *num)
{
    if (!num || !num->decimal) return 0;

    long long res = 0;
    long long mul = 1;
    // В number_value_value_new младший разряд (единицы) сохраняется в data[0],
    // поэтому идем от начала массива к концу.
    for (size_t i = 0; i < num->decimal->size; ++i)
    {
        res += num->decimal->data[i] * mul;
        mul *= 10;
    }

    return num->negative ? -res : res;
}

// Вспомогательная функция для создания number_value_t из целого числа
static number_value_t *number_value_from_int(long long value)
{
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%lld", value);
    // number_value_new скопирует данные из строки, поэтому локальный буфер безопасен
    return number_value_new(string_view_from(buffer), string_view_from(""));
}

static FILE *get_stream(const int fd)
{
    switch (fd)
    {
        case 0: return stdin;
        case 1: return stdout;
        case 2: return stderr;
        default: THROW("Unknown file descriptor");
    }
}

static value_t _write(context_t *context)
{
    const context_item_t *fd_item = context_get(context, string_view_from("fd"));
    const context_item_t *data_item = context_get(context, string_view_from("data"));

    if (!fd_item)
    {
        THROW("Missing argument: fd");
    }
    if (fd_item->value.type != VALUE_TYPE_NUMBER)
    {
        THROW("File descriptor should be a number");
    }
    if (!data_item)
    {
        THROW("Missing argument: data");
    }

    // Извлекаем дескриптор через вспомогательную функцию
    const int fd = (int) number_value_to_int(fd_item->value.data.as_number);

    if (0 <= fd && fd <= 2)
    {
        FILE *stream = get_stream(fd);
        value_print(stream, data_item->value);
        fflush(stream);
        return value_new_number(number_value_from_int(1));
    }

    // Для произвольного fd преобразуем значение в строку
    char *str = value_to_string(data_item->value);
    const ssize_t written = write(fd, str, strlen(str));
    free(str);
    return value_new_number(number_value_from_int(written));
}

static value_t _read(context_t *context)
{
    const context_item_t *fd_item = context_get(context, string_view_from("fd"));
    const context_item_t *size_item = context_get(context, string_view_from("size"));

    if (!fd_item)
    {
        THROW("Missing argument: fd");
    }
    if (fd_item->value.type != VALUE_TYPE_NUMBER)
    {
        THROW("File descriptor should be a number");
    }

    // Извлекаем дескриптор через вспомогательную функцию
    const int fd = (int) number_value_to_int(fd_item->value.data.as_number);

    size_t size = 1024;
    if (size_item)
    {
        if (size_item->value.type != VALUE_TYPE_NUMBER)
        {
            THROW("Size should be a number");
        }
        // Извлекаем размер через вспомогательную функцию
        size = (size_t) number_value_to_int(size_item->value.data.as_number);
    }

    if (0 <= fd && fd <= 2)
    {
        char *line = nullptr;
        size_t capacity = 0;
        ssize_t length = getline(&line, &capacity, get_stream(fd));
        if (length != -1)
        {
            if (length > 0 && line[length - 1] == '\n')
            {
                line[length - 1] = '\0';
                length--;
            }
            const auto result = value_new_string(string_value_from(line, (size_t) length));
            free(line);
            return result;
        }
        free(line);
        return value_new_string(string_value_from("", 0));
    }

    char *buffer = (char *) malloc(size + 1);
    if (!buffer)
        THROW("Memory allocation failed");
    const ssize_t bytes_read = read(fd, buffer, size);
    if (bytes_read > 0)
    {
        buffer[bytes_read] = '\0';
        const auto result = value_new_string(string_value_from(buffer, (size_t) bytes_read));
        free(buffer);
        return result;
    }
    free(buffer);
    return value_new_string(string_value_from("", 0));
}

static value_t _open(context_t *context)
{
    const context_item_t *filename_item = context_get(context, string_view_from("filename"));
    const context_item_t *mode_item = context_get(context, string_view_from("mode"));

    if (!filename_item || !mode_item ||
        filename_item->value.type != VALUE_TYPE_STRING ||
        mode_item->value.type != VALUE_TYPE_STRING)
    {
        return value_new_number(number_value_from_int(-1));
    }

    const char *filename = filename_item->value.data.as_string->data;
    const char *mode = mode_item->value.data.as_string->data;

    int flags = O_RDONLY;
    if (strcmp(mode, "w") == 0 || strcmp(mode, "w+") == 0 || strcmp(mode, "a") == 0 || strcmp(mode, "a+") == 0)
    {
        flags = O_WRONLY | O_CREAT | ((strcmp(mode, "a") == 0 || strcmp(mode, "a+") == 0) ? O_APPEND : O_TRUNC);
    } else if (strcmp(mode, "r+") == 0)
    {
        flags = O_RDWR;
    } else
    {
        return value_new_number(number_value_from_int(-1));
    }

    int fd = open(filename, flags, 0644);
    return value_new_number(number_value_from_int(fd));
}

static value_t _close(context_t *context)
{
    const context_item_t *fd_item = context_get(context, string_view_from("fd"));

    if (!fd_item)
    {
        THROW("Missing argument: fd");
    }
    if (fd_item->value.type != VALUE_TYPE_NUMBER)
    {
        THROW("File descriptor should be a number");
    }

    // Извлекаем дескриптор через вспомогательную функцию
    int fd = (int) number_value_to_int(fd_item->value.data.as_number);

    if (fd > 2)
    {
        close(fd);
    }
    return value_new_number(number_value_from_int(1));
}

void init_lib(context_t *context)
{
    REG_FUNC(_write, "fd", "data");
    REG_FUNC(_read, "fd", "size");
    REG_FUNC(_open, "filename", "mode");
    REG_FUNC(_close, "fd");
}
