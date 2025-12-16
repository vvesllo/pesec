#pragma once

#include "include/frontend/ast/ASTNode.hpp"
#include <memory>


class BreakNode final : public ASTNode
{
private:
    std::unique_ptr<ASTNode> m_value;

public:
    BreakNode(std::unique_ptr<ASTNode> value);

    Value evaluate(Context& context) const override;
    std::unique_ptr<ASTNode> clone() const override;
};