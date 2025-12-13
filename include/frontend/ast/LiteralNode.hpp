#pragma once

#include "include/frontend/ast/ASTNode.hpp"


class LiteralNode final : public ASTNode
{
private:
    const Value m_value;

public:
    LiteralNode();
    LiteralNode(const Value& value);
    Value evaluate(Context& context) const override;
    std::unique_ptr<ASTNode> clone() const override;
};