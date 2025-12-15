#include "include/frontend/ast/ArrayAccessAssignmentNode.hpp"

#include <stdexcept>


ArrayAccessAssignmentNode::ArrayAccessAssignmentNode(
    const std::string& name,
    std::vector<std::unique_ptr<ASTNode>> indices,
    std::unique_ptr<ASTNode> expression
)
    : m_name(name)
    , m_indices(std::move(indices))
    , m_expression(std::move(expression))
{
    
}


Value ArrayAccessAssignmentNode::evaluate(Context& context) const
{
    Value* current = &context.get(m_name).value;
    Value value = m_expression->evaluate(context);

    for (size_t i = 0; i < m_indices.size() - 1; ++i)
    {
        if (!current->isArray())
            throw std::runtime_error("Expected array at intermediate index access");

        long double index = m_indices[i]->evaluate(context).getDouble();

        if (index >= current->getArray().size())
            throw std::out_of_range("Array index out of bounds");

        current = &current->getArray().at(index);
    }

    if (!current->isArray())
        throw std::runtime_error("Final container is not an array");

    long double last_index = m_indices.back()->evaluate(context).getDouble();

    current->getArray().at(last_index) = value;

    return value;
}

std::unique_ptr<ASTNode> ArrayAccessAssignmentNode::clone() const
{
    std::vector<std::unique_ptr<ASTNode>> indices;
    
    for (auto& index : m_indices)
        indices.emplace_back(index ? index->clone() : nullptr);


    return std::make_unique<ArrayAccessAssignmentNode>(
        m_name,
        std::move(indices),
        m_expression ? m_expression->clone() : nullptr
    );
}