#pragma once

#include "include/frontend/ast/ASTNode.hpp"



class ArrayNode final : public ASTNode
{
private:
    std::vector<std::unique_ptr<ASTNode>> m_values;
    
public:
    ArrayNode(std::vector<std::unique_ptr<ASTNode>> values);

    Value evaluate(Context& context) const override;
    std::unique_ptr<ASTNode> clone() const override;
};