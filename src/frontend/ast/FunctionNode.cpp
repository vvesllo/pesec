#include "include/frontend/ast/FunctionNode.hpp"
#include "include/frontend/FunctionValue.hpp"


FunctionNode::FunctionNode(
    std::vector<std::string> parameters,
    std::unique_ptr<ASTNode> body
)
    : m_parameters(parameters)
    , m_body(std::move(body))
{

}

Value FunctionNode::evaluate(Context& context) const
{
    FunctionValue func(
        m_parameters,
        std::make_unique<Context>(&context),
        std::move(m_body)
    );
    
    return Value(std::move(func));
}

std::unique_ptr<ASTNode> FunctionNode::clone() const
{
    return std::make_unique<FunctionNode>(
        m_parameters,
        m_body ? m_body->clone() : nullptr
    );
}