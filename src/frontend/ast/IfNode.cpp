#include "include/frontend/ast/IfNode.hpp"

#include <stdexcept>


IfNode::IfNode(
    std::unique_ptr<ASTNode> condition,
    std::unique_ptr<ASTNode> then_block,
    std::unique_ptr<ASTNode> else_block
)
    : m_condition(std::move(condition))
    , m_then_block(std::move(then_block))
    , m_else_block(std::move(else_block))
{

}

Value IfNode::evaluate(Context& context) const
{
    Value condition_value = m_condition->evaluate(context);
    
    if (!condition_value.isBoolean())
        throw std::runtime_error("Condition must be boolean value");
    
    if (condition_value.getBoolean())
        return m_then_block->evaluate(context);
    else if (m_else_block)
        return m_else_block->evaluate(context);
    
    return Value();
}
