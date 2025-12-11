#pragma once

#include <unordered_map>
#include "include/frontend/Value.hpp"

struct Variable
{
    Value value;
    bool is_mutable;
};

struct Context final
{    
    std::unordered_map<std::string, Variable> variables;
};