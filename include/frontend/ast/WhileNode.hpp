#pragma once

#include "include/frontend/ast/ASTNode.hpp"
#include <memory>


class WhileNode final : public ASTNode
{
private:
    std::unique_ptr<ASTNode> m_condition;
    std::unique_ptr<ASTNode> m_while_block;
    std::unique_ptr<ASTNode> m_else_block;

public:
    WhileNode(
        std::unique_ptr<ASTNode> condition,
        std::unique_ptr<ASTNode> while_block,
        std::unique_ptr<ASTNode> else_block
    );
    
    Value evaluate(Context& context) const override;
};