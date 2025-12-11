#pragma once

#include "include/frontend/Value.hpp"
#include "include/frontend/Context.hpp"


class ASTNode
{
public:
    virtual ~ASTNode()=default;
    virtual Value evaluate(Context& context) const=0;
};