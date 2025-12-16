#pragma once

#include "include/frontend/ast/ASTNode.hpp"



class ArrayRangeNode final : public ASTNode
{
private:
    std::unique_ptr<ASTNode> m_begin;
    std::unique_ptr<ASTNode> m_end;
    bool m_contains_last;
    
public:
    ArrayRangeNode(
        std::unique_ptr<ASTNode> begin,
        std::unique_ptr<ASTNode> end,
    bool contains_last
    );

    Value evaluate(Context& context) const override;
    std::unique_ptr<ASTNode> clone() const override;
};