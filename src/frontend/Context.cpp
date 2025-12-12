#include "include/frontend/Context.hpp"

#include <stdexcept>


Context::Context()
    : m_parent(nullptr)
{   
}

Context::Context(Context* parent)
    : m_parent(parent)
{
}

bool Context::isExists(const std::string& name) const   
{
    if (m_variables.find(name) != m_variables.end())
        return true;
    
    if (m_parent)
        return m_parent->isExists(name);
    
    return false;
}

Variable Context::get(const std::string& name) const
{    
    auto it = m_variables.find(name);
    if (it != m_variables.end()) 
        return it->second;
    
    if (m_parent) 
        return m_parent->get(name);

    throw std::runtime_error("Variable does not exist");
}

void Context::set(const std::string& name, const Value& value)
{
    auto it = m_variables.find(name.data());
    
    if (it != m_variables.end()) 
    {
        if (!it->second.is_mutable)
            throw std::runtime_error("Variable is constant and can't be changed");
        
        it->second.value = value;
    } 
    
    else if (m_parent) 
        m_parent->set(name, value);

    else
        throw std::runtime_error("Attempt to assign undefined variable");
}

void Context::define(const std::string& name, const Variable& variable)
{
    auto it = m_variables.find(name);
    if (it != m_variables.end())
        throw std::runtime_error("Variable already exists");
    
    m_variables[name] = variable;
}