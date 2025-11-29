#pragma once

#include "include/frontend/ast/ASTNode.hpp"

#include <memory>

class BinaryOpNode : public ASTNode
{
private:
    const std::string m_op;
    std::unique_ptr<ASTNode> m_left;
    std::unique_ptr<ASTNode> m_right;

public:
    BinaryOpNode(
        const std::string& op,
        std::unique_ptr<ASTNode> left,
        std::unique_ptr<ASTNode> right
    );

    Value evaluate() override;
};