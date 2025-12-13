#include "include/frontend/ast/FunctionCallNode.hpp"

#include <stdexcept>
#include <print>


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

    if (value.isFunction())
        return value.getFunction()(context, values);

    return value.getFunction()(context, values);
}