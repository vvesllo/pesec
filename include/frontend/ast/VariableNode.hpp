#pragma once

#include <string>
#include "include/frontend/ast/ASTNode.hpp"



class VariableNode final : public ASTNode
{
private:
    const std::string m_name;
    
public:
    VariableNode(const std::string& name);

    Value evaluate(Context& context) const override;
    std::unique_ptr<ASTNode> clone() const override;
};