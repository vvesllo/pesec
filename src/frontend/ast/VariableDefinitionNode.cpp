#include "include/frontend/ast/VariableDefinitionNode.hpp"

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
    Value value = m_expression 
            ? m_expression->evaluate(context) 
            : Value();

    std::unique_ptr<Variable> variable = std::make_unique<Variable>(
        Value(value),
        m_is_mutable
    );
    
    context.define(m_name, std::move(variable));

    return value;
}

std::unique_ptr<ASTNode> VariableDefinitionNode::clone() const
{
    return std::make_unique<VariableDefinitionNode>(
        m_name,
        m_expression ? m_expression->clone() : nullptr,
        m_is_mutable
    );
}