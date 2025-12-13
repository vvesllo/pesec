#include "include/frontend/ast/VariableAssignmentNode.hpp"


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
    Value value = m_expression 
            ? m_expression->evaluate(context)
            : Value();
    context.set(m_name, value);

    return value;
}

std::unique_ptr<ASTNode> VariableAssignmentNode::clone() const
{
    return std::make_unique<VariableAssignmentNode>(
        m_name,
        m_expression ? m_expression->clone() : nullptr
    );
}