#include "include/frontend/ast/ArrayRangeNode.hpp"

ArrayRangeNode::ArrayRangeNode(
    std::unique_ptr<ASTNode> begin,
    std::unique_ptr<ASTNode> end,
    bool contains_last
)
    : m_begin(std::move(begin))
    , m_end(std::move(end))
    , m_contains_last(contains_last)
{
    
}

Value ArrayRangeNode::evaluate(Context& context) const
{
    Value begin_value = m_begin->evaluate(context);
    Value end_value = m_end->evaluate(context);
    
    std::vector<Value> values;

    for (long double i=begin_value.getDouble(); i < end_value.getDouble() + (m_contains_last ? 1 : 0); i++)
    {
        values.emplace_back(Value(i));
    }

    return values;
}

std::unique_ptr<ASTNode> ArrayRangeNode::clone() const
{
    return std::make_unique<ArrayRangeNode>(
        m_begin ? m_begin->clone() : nullptr,
        m_end ? m_end->clone() : nullptr,
        m_contains_last
    );
} 