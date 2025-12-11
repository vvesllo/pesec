#pragma once

#include "include/frontend/ast/ASTNode.hpp"
#include <memory>


class BlockNode final : public ASTNode
{
private:
    const std::vector<std::unique_ptr<ASTNode>> m_statements;

public:
    BlockNode(std::vector<std::unique_ptr<ASTNode>> statements);
    Value evaluate(Context& context) const override;
};