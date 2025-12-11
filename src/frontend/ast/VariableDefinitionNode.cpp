#include "include/frontend/ast/VariableDefinitionNode.hpp"

#include <stdexcept>


VariableDefinitionNode::VariableDefinitionNode(
    const std::string& name,
    std::unique_ptr<ASTNode> expression,
    bool is_mutable
)
    : m_name(name)
    , m_expression(std::move(expression))
    , m_is_mutable(is_mutable)
{
    
}

Value VariableDefinitionNode::evaluate(Context& context) const
{
    auto it = context.variables.find(m_name);
    if (it != context.variables.end())
        throw std::runtime_error("Variable '" + m_name + "' already exists");

    Variable var {
        .value=Value(),
        .is_mutable=m_is_mutable
    };
    

    if (m_expression)
        var.value = m_expression->evaluate(context);

    context.variables[m_name] = var;

    return var.value;
}