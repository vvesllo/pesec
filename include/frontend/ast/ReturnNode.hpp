#pragma once

#include "include/frontend/ast/ASTNode.hpp"
#include <memory>


class ReturnNode final : public ASTNode
{
private:
    std::unique_ptr<ASTNode> m_value;

public:
    ReturnNode(std::unique_ptr<ASTNode> value);

    Value evaluate(Context& context) const override;
};