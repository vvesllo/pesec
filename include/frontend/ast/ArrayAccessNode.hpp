#pragma once

#include "include/frontend/ast/ASTNode.hpp"


class ArrayAccessNode final : public ASTNode
{
private:
    const std::string m_name;
    std::unique_ptr<ASTNode> m_index;
    std::unique_ptr<ASTNode> m_expression;

public:
    ArrayAccessNode(
        const std::string& name, 
        std::unique_ptr<ASTNode> index
    );
    ArrayAccessNode(
        std::unique_ptr<ASTNode> expression,
        std::unique_ptr<ASTNode> index
    );

    Value evaluate(Context& context) const override;
    std::unique_ptr<ASTNode> clone() const override;
};