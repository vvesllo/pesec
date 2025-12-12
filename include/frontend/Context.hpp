#pragma once

#include <unordered_map>
#include "include/frontend/Value.hpp"

struct Variable
{
    Value value;
    bool is_mutable;
};

class Context final
{    
private:
    std::unordered_map<std::string, Variable> m_variables;

    Context* m_parent;

public:
    Context();
    Context(Context* parent);

    bool isExists(const std::string& name) const;
    
    Variable get(const std::string& name) const;
    void set(const std::string& name, const Value& value);
    void define(const std::string& name, const Variable& variable);
};