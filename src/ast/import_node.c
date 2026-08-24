#include "include/ast/import_node.h"

#include "include/ast/ast_node.h"
#include <stdlib.h>
#include <dlfcn.h>

#include "include/utils/throw.h"

#include "include/lexer.h"
#include "include/module_value.h"
#include "include/utils/execute_file.h"


#include <string.h>

static string_view_t get_file_extension(const char *path)
{
    const char *dot = strrchr(path, '.');
    const char *slash = strrchr(path, '/');
    const char *backslash = strrchr(path, '\\');

    if (!dot) return (string_view_t){.data = "", .length = 0};

    if (slash && dot < slash) return (string_view_t){.data = "", .length = 0};
    if (backslash && dot < backslash) return (string_view_t){.data = "", .length = 0};

    if (dot == path || (slash && dot == slash + 1) || (backslash && dot == backslash + 1))
        return (string_view_t){.data = "", .length = 0};

    const char *ext_start = dot + 1;
    const ull_t ext_len = strlen(ext_start);

    return (string_view_t){ .data = ext_start, .length = ext_len };
}

ast_node_t* import_node_new(ast_node_t* source)
{
    const auto node = (ast_node_t*)malloc(sizeof(ast_node_t));

    node->type = AST_NODE_IMPORT;
    node->node._import = (import_node_t*)malloc(sizeof(import_node_t));
    node->node._import->source = source;

    return node;
}

void import_node_free(import_node_t* import_node)
{
    ast_node_free(import_node->source);
    free(import_node);
}

value_t import_node_evaluate(const import_node_t* import_node, context_t* context)
{
    const value_t source_value = ast_node_evaluate(import_node->source, context);

    if (source_value.type != VALUE_TYPE_STRING)
        THROW("Import path should be a string");

    const string_value_t* path = source_value.data.as_string;
    context_t* module_context = context_new(nullptr);

    if (string_view_equals_cstr(get_file_extension(path->data), "pesec"))
    {
        execute_file(path->data, module_context);
    }
    else if (string_view_equals_cstr(get_file_extension(path->data), "so"))
    {
        void *handle = dlopen(path->data, RTLD_LAZY);
        char *error;

        if (!handle) THROW("%s\n", dlerror());

        void (*init_lib)(context_t*) = dlsym(handle, "init_lib");

        if ((error = dlerror()) != nullptr)
        {
            dlclose(handle);
            THROW("%s\n", error);
        }

        init_lib(module_context);

        dlclose(handle);
    }


    return MAKE_VAL_MODULE(module_value_new(module_context));
}
