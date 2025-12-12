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
    context.set(
        m_name,
        m_expression 
            ? m_expression->evaluate(context)
            : Value()
    );

    return Value();
}