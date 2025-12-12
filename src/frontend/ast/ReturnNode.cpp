#include "include/frontend/ast/ReturnNode.hpp"
#include "include/frontend/ast/ReturnValueException.hpp"


ReturnNode::ReturnNode(std::unique_ptr<ASTNode> value)
    : m_value(std::move(value))
{

}

Value ReturnNode::evaluate(Context& context) const
{
    Value return_value = Value();
    if (m_value) 
        return_value = m_value->evaluate(context);
    throw ReturnValueException(return_value);
}