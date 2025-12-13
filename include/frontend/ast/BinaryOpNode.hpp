#pragma once

#include <memory>
#include <string>
#include "include/frontend/ast/ASTNode.hpp"
#include "include/frontend/Token.hpp"


class BinaryOpNode final : public ASTNode
{
private:
    const TokenAny m_op_token;
    std::unique_ptr<ASTNode> m_lhs;
    std::unique_ptr<ASTNode> m_rhs;

    template<class T>
    bool match() const;

public:
    BinaryOpNode(
        const TokenAny& op_token,
        std::unique_ptr<ASTNode> lhs,
        std::unique_ptr<ASTNode> rhs
    );
    Value evaluate(Context& context) const override;
    std::unique_ptr<ASTNode> clone() const override;
};