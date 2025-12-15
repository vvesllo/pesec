#include "include/frontend/ast/ArrayAccessNode.hpp"

#include <stdexcept>

ArrayAccessNode::ArrayAccessNode(
    const std::string& name, 
    std::unique_ptr<ASTNode> index
)
    : m_name(name)
    , m_expression(nullptr)
    , m_index(std::move(index))
{

}

ArrayAccessNode::ArrayAccessNode(
    std::unique_ptr<ASTNode> expression,
    std::unique_ptr<ASTNode> index
)
    : m_name("")
    , m_expression(std::move(expression))
    , m_index(std::move(index))
{

}

Value ArrayAccessNode::evaluate(Context& context) const
{
    Value value;
    if (m_expression)
        value = m_expression->evaluate(context);
    else
        value = context.get(m_name).value;


    if (!value.isArray())
        throw std::runtime_error("Can't get access for non-array");

    ArrayValue& array_value = value.getArray();
    
    Value index_value = m_index->evaluate(context);
   
    if (!index_value.isDouble())
        throw std::runtime_error("Index value must be numeric");
    
    long double index = index_value.getDouble();

    return array_value[index];
}

std::unique_ptr<ASTNode> ArrayAccessNode::clone() const
{
    if (m_expression)
        return std::make_unique<ArrayAccessNode>(
            m_expression->clone(),
            m_index ? m_index->clone() : nullptr
        );
    return std::make_unique<ArrayAccessNode>(
        m_name,
        m_index ? m_index->clone() : nullptr
    );
}