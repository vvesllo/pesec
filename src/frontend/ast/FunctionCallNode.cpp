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
    auto it = context.variables.find(m_name);
    if (it == context.variables.end())
        throw std::runtime_error("Function '" + m_name + "' not found");

    std::vector<Value> values;

    for (const auto& argument : m_arguments)
        values.emplace_back(argument->evaluate(context));

    return it->second.value.getFunction()(context, values);
}