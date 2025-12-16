#include "include/frontend/ast/ArrayRangeNode.hpp"
#include <stdexcept>


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

    if (!begin_value.isDouble() || !end_value.isDouble())
        throw std::runtime_error("Range begin and end should be numeric");
    
    long double begin = begin_value.getDouble();
    long double end = end_value.getDouble();

    if (begin < end)
        for (long double i=begin; i < end + (m_contains_last ? 1 : 0); i++)
            values.emplace_back(Value(i));
    else
        for (long double i=begin; i > end - (m_contains_last ? 1 : 0); i--)
            values.emplace_back(Value(i));

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