#pragma once

#include "include/frontend/ast/ASTNode.hpp"
#include <memory>


class IfNode final : public ASTNode
{
private:
    std::unique_ptr<ASTNode> m_condition;
    std::unique_ptr<ASTNode> m_then_block;
    std::unique_ptr<ASTNode> m_else_block;

public:
    IfNode(
        std::unique_ptr<ASTNode> condition,
        std::unique_ptr<ASTNode> then_block,
        std::unique_ptr<ASTNode> else_block
    );
    
    Value evaluate(Context& context) const override;
    std::unique_ptr<ASTNode> clone() const override;
};