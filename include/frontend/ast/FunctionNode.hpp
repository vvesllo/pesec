#pragma once

#include "include/frontend/ast/ASTNode.hpp"
#include <memory>
#include <vector>
#include <string>


class FunctionNode final : public ASTNode
{
private:
    const std::vector<std::string> m_parameters;
    mutable std::unique_ptr<ASTNode> m_body;

public:
    FunctionNode(
        const std::vector<std::string>& parameters,
        std::unique_ptr<ASTNode> body
    );

    Value evaluate(Context& context) const override;
};