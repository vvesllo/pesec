#pragma once

#include <memory>
#include <vector>
#include <string>
#include "include/frontend/ast/ASTNode.hpp"



class VariableAssignmentNode final : public ASTNode
{
private:
    const std::string m_name;
    std::unique_ptr<ASTNode> m_expression;

public:
    VariableAssignmentNode(
        const std::string& name,
        std::unique_ptr<ASTNode> expression
    );

    Value evaluate(Context& context) const override;
};