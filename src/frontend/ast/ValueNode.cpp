#include "include/frontend/ast/ValueNode.hpp"


ValueNode::ValueNode()
    : m_value()
{

}

ValueNode::ValueNode(const Value& value)
    : m_value(value)
{

}

Value ValueNode::evaluate(Context& context) const
{
    return m_value;
}

std::unique_ptr<ASTNode> ValueNode::clone() const
{
    return std::make_unique<ValueNode>(m_value);
}