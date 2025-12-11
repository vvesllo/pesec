#pragma once

#include <memory>
#include <string>
#include "include/frontend/ast/ASTNode.hpp"



class VariableDefinitionNode final : public ASTNode
{
private:
    const std::string m_name;
    std::unique_ptr<ASTNode> m_expression;
    bool m_is_mutable;
    
public:
    VariableDefinitionNode(
        const std::string& name,
        std::unique_ptr<ASTNode> expression,
        bool is_mutable
    );

    Value evaluate(Context& context) const override;
};