#pragma once

#include "include/frontend/ast/ASTNode.hpp"

class IntegerNode : public ASTNode
{
private:
    long long m_value;

public:
    IntegerNode(long long value);
    Value evaluate() override;
};