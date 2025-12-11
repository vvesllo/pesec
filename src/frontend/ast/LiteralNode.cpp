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