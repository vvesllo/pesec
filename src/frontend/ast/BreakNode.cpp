#include "include/frontend/ast/BreakNode.hpp"
#include "include/frontend/ast/BreakValueException.hpp"


BreakNode::BreakNode(std::unique_ptr<ASTNode> value)
    : m_value(std::move(value))
{

}

Value BreakNode::evaluate(Context& context) const
{
    if (m_value) 
        throw BreakValueException(m_value->evaluate(context));   
    throw BreakValueException(Value());
}