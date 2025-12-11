#include "include/frontend/ast/VariableNode.hpp"

#include <stdexcept>


VariableNode::VariableNode(const std::string& name)
    : m_name(name)
{
    
}


Value VariableNode::evaluate(Context& context) const
{
    auto it = context.variables.find(m_name);
    if (it == context.variables.end())
        throw std::runtime_error("Variable '" + m_name + "' not found");

    return it->second.value;
}