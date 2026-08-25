#include "../include/lib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "../../../include/function_value.h"
#include "../../../include/string_value.h"
#include "../../../include/value.h"
#include "include/utils/throw.h"

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
                FUNCTION_VALUE_TYPE_C_FUNCTION, \
                context \
            ) \
        ), \
        true \
    )

static FILE* get_stream(const int fd)
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

    // Проверка наличия и типа fd
    if (!fd_item) {
        THROW("Missing argument: fd");
    }
    if (fd_item->value.type != VALUE_TYPE_NUMBER) {
        THROW("File descriptor should be a number");
    }
    // Проверка наличия data
    if (!data_item) {
        THROW("Missing argument: data");
    }

    const int fd = (int)fd_item->value.data.as_number;

    // Для стандартных потоков используем stdio
    if (0 <= fd && fd <= 2)
    {
        FILE *stream = get_stream(fd);
        value_print(stream, data_item->value);
        fflush(stream);
        return MAKE_VAL_NUM(1);
    }

    // Для произвольных дескрипторов данные должны быть строкой
    if (data_item->value.type != VALUE_TYPE_STRING)
    {
        THROW("Data should be a string value");
    }

    const string_value_t *data_value = data_item->value.data.as_string;
    const ssize_t written = write(fd, data_value->data, data_value->size);

    return MAKE_VAL_NUM((long double)written);
}

static value_t _read(context_t *context)
{
    const context_item_t *fd_item = context_get(context, string_view_from("fd"));
    const context_item_t *size_item = context_get(context, string_view_from("size"));

    // Проверка fd
    if (!fd_item) {
        THROW("Missing argument: fd");
    }
    if (fd_item->value.type != VALUE_TYPE_NUMBER) {
        THROW("File descriptor should be a number");
    }

    const int fd = (int)fd_item->value.data.as_number;
    size_t size = 1024; // значение по умолчанию
    if (size_item) {
        if (size_item->value.type != VALUE_TYPE_NUMBER) {
            THROW("Size should be a number");
        }
        size = (size_t)size_item->value.data.as_number;
    }

    // Для stdin/stdout/stderr используем getline
    if (0 <= fd && fd <= 2)
    {
        char *line = nullptr;
        size_t capacity = 0;
        ssize_t length = getline(&line, &capacity, get_stream(fd));

        if (length != -1)
        {
            // Удаляем завершающий перевод строки, если есть
            if (length > 0 && line[length - 1] == '\n')
            {
                line[length - 1] = '\0';
                length--;
            }

            const auto result = MAKE_VAL_STR(string_value_from(line, (size_t)length));
            free(line);
            return result;
        }
        free(line);
        return MAKE_VAL_STR(string_value_from("", 0));
    }

    // Для обычных файловых дескрипторов
    char *buffer = (char*)malloc(size + 1);
    if (!buffer) THROW("Memory allocation failed");

    const ssize_t bytes_read = read(fd, buffer, size);
    if (bytes_read > 0)
    {
        buffer[bytes_read] = '\0';
        const auto result = MAKE_VAL_STR(string_value_from(buffer, (size_t)bytes_read));
        free(buffer);
        return result;
    }

    free(buffer);
    return MAKE_VAL_STR(string_value_from("", 0));
}

static value_t _open(context_t *context)
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
    } else {
        // Неподдерживаемый режим – возвращаем -1
        return MAKE_VAL_NUM(-1);
    }

    int fd = open(filename, flags, 0644);
    return MAKE_VAL_NUM((long double)fd);
}

static value_t _close(context_t *context)
{
    const context_item_t *fd_item = context_get(context, string_view_from("fd"));

    if (!fd_item) {
        THROW("Missing argument: fd");
    }
    if (fd_item->value.type != VALUE_TYPE_NUMBER) {
        THROW("File descriptor should be a number");
    }

    int fd = (int)fd_item->value.data.as_number;

    // Закрываем только нестандартные дескрипторы
    if (fd > 2) {
        close(fd);
    }

    return MAKE_VAL_NUM(1);
}

void init_lib(context_t *context)
{
    REG_FUNC(_write, "fd", "data");
    REG_FUNC(_read, "fd", "size");
    REG_FUNC(_open, "filename", "mode");
    REG_FUNC(_close, "fd");
}