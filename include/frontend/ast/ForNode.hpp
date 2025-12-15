#pragma once

#include "include/frontend/ast/ASTNode.hpp"
#include <memory>


class ForNode final : public ASTNode
{
private:
    const std::string m_iterator;
    std::unique_ptr<ASTNode> m_iterable;
    std::unique_ptr<ASTNode> m_for_block;
    std::unique_ptr<ASTNode> m_else_block;

public:
    ForNode(
        const std::string& iterator,
        std::unique_ptr<ASTNode> iterable,
        std::unique_ptr<ASTNode> for_block,
        std::unique_ptr<ASTNode> else_block
    );
    
    Value evaluate(Context& context) const override;
    std::unique_ptr<ASTNode> clone() const override;
};