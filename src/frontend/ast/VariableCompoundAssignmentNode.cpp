#include "include/frontend/ast/VariableCompoundAssignmentNode.hpp"

#include <stdexcept>


template<class T>
bool VariableCompoundAssignmentNode::match() const
{ 
    return std::holds_alternative<T>(m_op_token); 
}

VariableCompoundAssignmentNode::VariableCompoundAssignmentNode(
    const TokenAny& op_token,
    const std::string& name,
    std::unique_ptr<ASTNode> expression
)
    : m_op_token(op_token)
    , m_name(name)
    , m_expression(std::move(expression))
{
    
}

Value VariableCompoundAssignmentNode::evaluate(Context& context) const
{
    Value value = m_expression 
            ? m_expression->evaluate(context)
            : Value();
            
    if      (match<TokenType::PlusEquals>())     value = context.get(m_name).value + value;
    else if (match<TokenType::MinusEquals>())    value = context.get(m_name).value - value;
    else if (match<TokenType::AsteriskEquals>()) value = context.get(m_name).value * value;
    else if (match<TokenType::SlashEquals>())    value = context.get(m_name).value / value;
    else if (match<TokenType::PercentEquals>())  value = context.get(m_name).value % value;
    else throw std::runtime_error("Unknown compound assignment operator");

    context.set(m_name, value);

    return Value();
}

std::unique_ptr<ASTNode> VariableCompoundAssignmentNode::clone() const
{
    return std::make_unique<VariableCompoundAssignmentNode>(
        m_op_token,
        m_name,
        m_expression ? m_expression->clone() : nullptr
    );
}