#pragma once

#include "include/frontend/ast/ASTNode.hpp"

class StringNode : public ASTNode
{
private:
    const std::string m_value;

public:
    StringNode(const std::string& value);
    Value evaluate() override;
};