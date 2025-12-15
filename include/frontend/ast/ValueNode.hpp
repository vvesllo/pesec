#pragma once

#include "include/frontend/ast/ASTNode.hpp"


class ValueNode final : public ASTNode
{
private:
    const Value m_value;

public:
    ValueNode();
    ValueNode(const Value& value);
    Value evaluate(Context& context) const override;
    std::unique_ptr<ASTNode> clone() const override;
};