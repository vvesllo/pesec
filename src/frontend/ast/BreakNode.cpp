#include "include/frontend/ast/BreakNode.hpp"
#include "include/frontend/ast/BreakValueException.hpp"


BreakNode::BreakNode()
{

}

Value BreakNode::evaluate(Context& context) const
{
    throw BreakValueException(Value());
}

std::unique_ptr<ASTNode> BreakNode::clone() const
{
    return std::make_unique<BreakNode>();
}