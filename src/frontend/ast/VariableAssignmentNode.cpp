#include "include/frontend/ast/VariableAssignmentNode.hpp"

#include <stdexcept>


VariableAssignmentNode::VariableAssignmentNode(
    const std::string& name,
    std::unique_ptr<ASTNode> expression
)
    : m_name(name)
    , m_expression(std::move(expression))
{
    
}


Value VariableAssignmentNode::evaluate(Context& context) const
{
    auto it = context.variables.find(m_name);
    if (it == context.variables.end())
        throw std::runtime_error("Variable '" + m_name + "' not found");
    
    if (!it->second.is_mutable)
        throw std::runtime_error("Can't change a const value");

    context.variables[m_name].value = m_expression->evaluate(context);

    return it->second.value;
}