#include "include/frontend/ast/ReturnNode.hpp"
#include "include/frontend/ast/ReturnValueException.hpp"


ReturnNode::ReturnNode(std::unique_ptr<ASTNode> value)
    : m_value(std::move(value))
{

}

Value ReturnNode::evaluate(Context& context) const
{
    if (m_value)
        throw ReturnValueException(m_value->evaluate(context));
    throw ReturnValueException(Value());
}

std::unique_ptr<ASTNode> ReturnNode::clone() const
{
    return std::make_unique<ReturnNode>(
        m_value ? m_value->clone() : nullptr
    );
}