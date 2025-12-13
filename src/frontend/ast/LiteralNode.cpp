#include "include/frontend/ast/LiteralNode.hpp"


LiteralNode::LiteralNode()
    : m_value()
{

}

LiteralNode::LiteralNode(const Value& value)
    : m_value(value)
{

}

Value LiteralNode::evaluate(Context& context) const
{
    return m_value;
}

std::unique_ptr<ASTNode> LiteralNode::clone() const
{
    return std::make_unique<LiteralNode>(m_value);
}