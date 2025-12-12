#include "include/frontend/ast/WhileNode.hpp"
#include "include/frontend/ast/BreakValueException.hpp"

#include <stdexcept>


WhileNode::WhileNode(
    std::unique_ptr<ASTNode> condition,
    std::unique_ptr<ASTNode> while_block
)
    : m_condition(std::move(condition))
    , m_while_block(std::move(while_block))
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
            value = m_while_block->evaluate(context);
        } 
        catch (const BreakValueException& break_value) 
        {
            return break_value.value();
        }
    }
    
    return value;
}
