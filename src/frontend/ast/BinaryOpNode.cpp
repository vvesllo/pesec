#include "include/frontend/ast/BinaryOpNode.hpp"

#include <stdexcept>


template<class T>
bool BinaryOpNode::match() const 
{ 
    return std::holds_alternative<T>(m_op_token); 
}

BinaryOpNode::BinaryOpNode(
    const TokenAny& op_token,
    std::unique_ptr<ASTNode> lhs,
    std::unique_ptr<ASTNode> rhs
)
    : m_op_token(op_token)
    , m_lhs(std::move(lhs))
    , m_rhs(std::move(rhs))
{

}

Value BinaryOpNode::evaluate(Context& context) const
{
    const Value& lhs_value = m_lhs->evaluate(context);
    const Value& rhs_value = m_rhs->evaluate(context);

    if (match<TokenType::Plus>())                   return lhs_value + rhs_value;
    else if (match<TokenType::Minus>())             return lhs_value - rhs_value;
    else if (match<TokenType::Asterisk>())          return lhs_value * rhs_value;
    else if (match<TokenType::Slash>())             return lhs_value / rhs_value;
    else if (match<TokenType::Percent>())           return lhs_value % rhs_value;
    else if (match<TokenType::AsteriskAsterisk>())  return lhs_value.power(rhs_value);
    
    else if (match<TokenType::EqualsEquals>())  return lhs_value == rhs_value;
    else if (match<TokenType::NotEquals>())     return lhs_value != rhs_value;
    else if (match<TokenType::Less>())          return lhs_value <  rhs_value;
    else if (match<TokenType::Greater>())       return lhs_value >  rhs_value;
    else if (match<TokenType::LessEquals>())    return lhs_value <= rhs_value;
    else if (match<TokenType::GreaterEquals>()) return lhs_value >= rhs_value;
    
    throw std::runtime_error("Unknown operation");
}

std::unique_ptr<ASTNode> BinaryOpNode::clone() const
{
    return std::make_unique<BinaryOpNode>(
        m_op_token,
        m_lhs ? m_lhs->clone() : nullptr,
        m_rhs ? m_rhs->clone() : nullptr
    );
}