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
    Variable var {
        .value=m_expression 
            ? m_expression->evaluate(context) 
            : Value(),
        .is_mutable=m_is_mutable
    };
    
    context.define(m_name, var);

    return var.value;
}