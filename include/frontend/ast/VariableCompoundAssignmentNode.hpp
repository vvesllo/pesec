#pragma once

#include <memory>
#include <string>
#include "include/frontend/ast/ASTNode.hpp"
#include "include/frontend/Token.hpp"


class VariableCompoundAssignmentNode final : public ASTNode
{
private:
    const TokenAny m_op_token;
    const std::string m_name;
    std::unique_ptr<ASTNode> m_expression;

    template<class T>
    bool match() const;

public:
    VariableCompoundAssignmentNode(
        const TokenAny& op_token,
        const std::string& name,
        std::unique_ptr<ASTNode> expression
    );

    Value evaluate(Context& context) const override;
    std::unique_ptr<ASTNode> clone() const override;
};