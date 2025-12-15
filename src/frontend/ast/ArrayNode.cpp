#include "include/frontend/ast/ArrayNode.hpp"

ArrayNode::ArrayNode(std::vector<std::unique_ptr<ASTNode>> values)
    : m_values(std::move(values))
{
    
}


Value ArrayNode::evaluate(Context& context) const
{
    ArrayValue values;
    for (auto& value : m_values)
        values.emplace_back(value->evaluate(context));
    
    return values;
}

std::unique_ptr<ASTNode> ArrayNode::clone() const
{
    std::vector<std::unique_ptr<ASTNode>> values;
    for (auto& value : m_values)
        values.emplace_back(value ? value->clone() : nullptr);

    return std::make_unique<ArrayNode>(std::move(values));
} 