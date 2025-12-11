#pragma once

#include <memory>
#include <vector>
#include <string>
#include "include/frontend/ast/ASTNode.hpp"


class FunctionCallNode final : public ASTNode
{
private:
    const std::string m_name;
    std::vector<std::unique_ptr<ASTNode>> m_arguments;

public:
    FunctionCallNode(
        const std::string& name,
        std::vector<std::unique_ptr<ASTNode>> arguments
    );

    Value evaluate(Context& context) const override;
};