#pragma once

#include "include/frontend/ast/Value.hpp"

class ASTNode
{
public:
    virtual ~ASTNode()=default;
    virtual Value evaluate()=0;
};