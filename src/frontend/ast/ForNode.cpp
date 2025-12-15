#include "include/frontend/ast/ForNode.hpp"
#include "include/frontend/ast/ReturnValueException.hpp"

#include <stdexcept>

ForNode::ForNode(
    const std::string& iterator,
    std::unique_ptr<ASTNode> iterable,
    std::unique_ptr<ASTNode> for_block,
    std::unique_ptr<ASTNode> else_block
)
    : m_iterator(iterator)
    , m_iterable(std::move(iterable))
    , m_for_block(std::move(for_block))
    , m_else_block(std::move(else_block))
{

}

Value ForNode::evaluate(Context& context) const
{
    Value iterable_value = m_iterable->evaluate(context);
    
    if (!iterable_value.isArray())
        throw std::runtime_error("Can't iterate non-array value");


    ArrayValue& array = iterable_value.getArray();
    
    for (auto& element : array)
    {
        if (context.isExists(m_iterator))
            context.set(m_iterator, element);
        else
            context.define(m_iterator, std::make_unique<Variable>(element, true));
        
        try 
        {
            m_for_block->evaluate(context);
        }
        catch (const ReturnValueException& return_value) 
        {
            return return_value.value();
        }
    }
    
    if (m_else_block)
    {
        try 
        {
            return m_else_block->evaluate(context);
        } 
        catch (const ReturnValueException& return_value) 
        {
            return return_value.value();
        }
    }

    return Value();
}

std::unique_ptr<ASTNode> ForNode::clone() const
{
    return std::make_unique<ForNode>(
        m_iterator,
        m_iterable ? m_iterable->clone() : nullptr,
        m_for_block ? m_for_block->clone() : nullptr,
        m_else_block ? m_else_block->clone() : nullptr
    );
}