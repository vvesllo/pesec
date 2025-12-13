#include "include/frontend/ast/FunctionInstantCallNode.hpp"
#include "include/frontend/FunctionValue.hpp"


FunctionInstantCallNode::FunctionInstantCallNode(
    std::vector<std::string> parameters,
    std::unique_ptr<ASTNode> body,
    std::vector<std::unique_ptr<ASTNode>> arguments
)
    : m_parameters(parameters)
    , m_body(std::move(body))
    , m_arguments(std::move(arguments))
{
    
}

Value FunctionInstantCallNode::evaluate(Context& context) const
{
    std::vector<Value> values;

    for (const auto& argument : m_arguments)
        values.emplace_back(argument->evaluate(context));

    FunctionValue function (
        m_parameters,
        std::make_shared<Context>(&context),
        std::shared_ptr<ASTNode>(m_body->clone())
    );

    return function(context, values);
}

std::unique_ptr<ASTNode> FunctionInstantCallNode::clone() const
{
    std::vector<std::unique_ptr<ASTNode>> arguments;

    for (auto& argument : m_arguments)
        arguments.emplace_back(argument ? argument->clone() : nullptr);

    return std::make_unique<FunctionInstantCallNode>(
        m_parameters,
        m_body ? m_body->clone() : nullptr,
        std::move(arguments)
    );
}