#include "include/frontend/ast/WhileNode.hpp"
#include "include/frontend/ast/BreakValueException.hpp"

#include <stdexcept>

WhileNode::WhileNode(
    std::unique_ptr<ASTNode> condition,
    std::unique_ptr<ASTNode> while_block,
    std::unique_ptr<ASTNode> else_block
)
    : m_condition(std::move(condition))
    , m_while_block(std::move(while_block))
    , m_else_block(std::move(else_block))
{

}

Value WhileNode::evaluate(Context& context) const
{
    Value value = m_condition->evaluate(context);
    
    if (!value.isBoolean())
        throw std::runtime_error("Condition must be boolean value");
    
    while (m_condition->evaluate(context).getBoolean())
    {
        try 
        {
            m_while_block->evaluate(context);
        }
        catch (const BreakValueException& break_value) 
        {
            return break_value.value();
        }
    }
    
    if (m_else_block)
        return m_else_block->evaluate(context);

    return Value();
}

std::unique_ptr<ASTNode> WhileNode::clone() const
{
    return std::make_unique<WhileNode>(
        m_condition ? m_condition->clone() : nullptr,
        m_while_block ? m_while_block->clone() : nullptr,
        m_else_block ? m_else_block->clone() : nullptr
    );
}