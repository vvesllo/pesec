#pragma once

#include "include/frontend/ast/ASTNode.hpp"
#include <memory>


class BreakNode final : public ASTNode
{
public:
    BreakNode();

    Value evaluate(Context& context) const override;
    std::unique_ptr<ASTNode> clone() const override;
};