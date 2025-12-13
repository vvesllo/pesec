#pragma once

#include <memory>
#include <vector>
#include <string>
#include "include/frontend/ast/ASTNode.hpp"


class FunctionInstantCallNode final : public ASTNode
{
private:
    std::vector<std::string> m_parameters;
    std::unique_ptr<ASTNode> m_body;
    std::vector<std::unique_ptr<ASTNode>> m_arguments;

public:
    FunctionInstantCallNode(
        std::vector<std::string> parameters,
        std::unique_ptr<ASTNode> body,
        std::vector<std::unique_ptr<ASTNode>> arguments
    );

    Value evaluate(Context& context) const override;
    std::unique_ptr<ASTNode> clone() const override;
};