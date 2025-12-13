#include "include/frontend/ast/FunctionCallNode.hpp"

#include <stdexcept>


FunctionCallNode::FunctionCallNode(
    const std::string& name,
    std::vector<std::unique_ptr<ASTNode>> arguments
)
    : m_name(name)
    , m_arguments(std::move(arguments))
{
    
}

Value FunctionCallNode::evaluate(Context& context) const
{
    std::vector<Value> values;
    Variable& variable = context.get(m_name);
    Value& value = variable.value;

    for (const auto& argument : m_arguments)
        values.emplace_back(argument->evaluate(context));

    if (!value.isFunction())
        throw std::runtime_error("Variable is not a function");
    
    return value.getFunction()(context, values);
}

std::unique_ptr<ASTNode> FunctionCallNode::clone() const
{
    std::vector<std::unique_ptr<ASTNode>> arguments;

    for (auto& argument : m_arguments)
        arguments.emplace_back(argument ? argument->clone() : nullptr);

    return std::make_unique<FunctionCallNode>(
        m_name,
        std::move(arguments)
    );
}