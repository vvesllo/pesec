#pragma once

#include <memory>
#include "include/frontend/ast/ASTNode.hpp"



class ArrayAccessAssignmentNode final : public ASTNode
{
private:
    const std::string m_name;
    std::vector<std::unique_ptr<ASTNode>> m_indices;
    std::unique_ptr<ASTNode> m_expression;
    
public:
    ArrayAccessAssignmentNode(
        const std::string& name,
        std::vector<std::unique_ptr<ASTNode>> indices,
        std::unique_ptr<ASTNode> expression
    );

    Value evaluate(Context& context) const override;
    std::unique_ptr<ASTNode> clone() const override;
};