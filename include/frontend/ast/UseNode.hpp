#pragma once

#include "include/frontend/ast/ASTNode.hpp"


class UseNode final : public ASTNode
{
private:
    const std::string m_filepath;

public:
    UseNode(const std::string& filepath);
    Value evaluate(Context& context) const override;
    std::unique_ptr<ASTNode> clone() const override;
};