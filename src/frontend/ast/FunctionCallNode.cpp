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
    Value function_value = context.get(m_name).value;

    if (!function_value.isFunction())
        throw std::runtime_error("Variable is not a function");

    for (const auto& argument : m_arguments)
        values.emplace_back(argument->evaluate(context));

    return function_value.getFunction()(context, values);
}