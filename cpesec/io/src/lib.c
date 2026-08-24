#include "../include/lib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "../../../include/function_value.h"
#include "../../../include/string_value.h"
#include "../../../include/value.h"

// Макрос для регистрации C-функции в контексте интерпретатора
#define REG_FUNC(function_name, ...) \
    context_push( \
        context, \
        string_view_from(#function_name), \
        MAKE_VAL_FUNC( \
            function_value_new( \
                parameter_new_from_cstr((const char *[]){__VA_ARGS__, nullptr}), \
                (function_value_value_t){ \
                    .as_c_function = function_name \
                }, \
                FUNCTION_VALUE_TYPE_C_FUNCTION \
            ) \
        ), \
        true \
    )

static value_t builtin_write(context_t *context)
{
    const context_item_t *fd_item = context_get(context, string_view_from("fd"));
    const context_item_t *data_item = context_get(context, string_view_from("data"));

    if (!fd_item || !data_item) {
        return MAKE_VAL_NUM(-1);
    }

    int fd = (int)fd_item->value.data.as_number;

    // Для stdout (1) и stderr (2) используем нативный value_print для корректного
    // форматирования типов pesec (числа, строки, структуры и т.д.)
    if (fd == 1 || fd == 2) {
        FILE *stream = (fd == 1) ? stdout : stderr;
        value_print(stream, data_item->value);
        fflush(stream);
        return MAKE_VAL_NUM(1);
    }

    // Для обычных файлов записываем строковые данные
    if (data_item->value.type == VALUE_TYPE_STRING && data_item->value.data.as_string) {
        const string_value_t *str_val = data_item->value.data.as_string;
        ssize_t written = write(fd, str_val->data, str_val->size);
        return MAKE_VAL_NUM((long double)written);
    }

    return MAKE_VAL_NUM(-1);
}

static value_t builtin_read(context_t *context)
{
    const context_item_t *fd_item = context_get(context, string_view_from("fd"));
    const context_item_t *size_item = context_get(context, string_view_from("size"));

    if (!fd_item) {
        return MAKE_VAL_STR(string_value_from("", 0));
    }

    int fd = (int)fd_item->value.data.as_number;
    size_t size = size_item ? (size_t)size_item->value.data.as_number : 1024;

    // Чтение из stdin (0) с обработкой перевода строки
    if (fd == 0) {
        char *line = NULL;
        size_t capacity = 0;
        ssize_t length = getline(&line, &capacity, stdin);

        if (length != -1) {
            if (length > 0 && line[length - 1] == '\n') {
                line[length - 1] = '\0';
                length--;
            }
            value_t result = MAKE_VAL_STR(string_value_from(line, (size_t)length));
            free(line);
            return result;
        }
        free(line);
        return MAKE_VAL_STR(string_value_from("", 0));
    }

    // Чтение из файла
    char *buffer = (char *)malloc(size + 1);
    if (!buffer) {
        return MAKE_VAL_STR(string_value_from("Error: memory allocation failed", 33));
    }

    ssize_t bytes_read = read(fd, buffer, size);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        value_t result = MAKE_VAL_STR(string_value_from(buffer, (size_t)bytes_read));
        free(buffer);
        return result;
    }

    free(buffer);
    return MAKE_VAL_STR(string_value_from("", 0));
}

static value_t builtin_open(context_t *context)
{
    const context_item_t *filename_item = context_get(context, string_view_from("filename"));
    const context_item_t *mode_item = context_get(context, string_view_from("mode"));

    if (!filename_item || !mode_item ||
        filename_item->value.type != VALUE_TYPE_STRING ||
        mode_item->value.type != VALUE_TYPE_STRING) {
        return MAKE_VAL_NUM(-1);
    }

    const char *filename = filename_item->value.data.as_string->data;
    const char *mode = mode_item->value.data.as_string->data;

    int flags = O_RDONLY;
    if (strcmp(mode, "w") == 0 || strcmp(mode, "w+") == 0 || strcmp(mode, "a") == 0 || strcmp(mode, "a+") == 0) {
        flags = O_WRONLY | O_CREAT | ((strcmp(mode, "a") == 0 || strcmp(mode, "a+") == 0) ? O_APPEND : O_TRUNC);
    } else if (strcmp(mode, "r+") == 0) {
        flags = O_RDWR;
    }

    int fd = open(filename, flags, 0644);
    return MAKE_VAL_NUM((long double)fd);
}

static value_t builtin_close(context_t *context)
{
    const context_item_t *fd_item = context_get(context, string_view_from("fd"));

    if (!fd_item) {
        return MAKE_VAL_NUM(0);
    }

    int fd = (int)fd_item->value.data.as_number;

    if (fd > 2) {
        close(fd);
    }

    return MAKE_VAL_NUM(1);
}

void init_lib(context_t *context)
{
    // Регистрируем функции с именами, которые будут вызываться из кода на pesec
    REG_FUNC(builtin_write, "fd", "data");
    REG_FUNC(builtin_read, "fd", "size");
    REG_FUNC(builtin_open, "filename", "mode");
    REG_FUNC(builtin_close, "fd");
}