#include "include/frontend/ast/BlockNode.hpp"
#include "include/frontend/ast/ReturnNode.hpp"



BlockNode::BlockNode(std::vector<std::unique_ptr<ASTNode>> statements)
    : m_statements(std::move(statements))
{

}

Value BlockNode::evaluate(Context& context) const
{
    Context local_context(&context);
    for (auto& statement : m_statements)
    {
        Value value = statement->evaluate(local_context);
        if (dynamic_cast<ReturnNode*>(statement.get()))
            return value;
    }
    
    return Value();
}

std::unique_ptr<ASTNode> BlockNode::clone() const
{
    std::vector<std::unique_ptr<ASTNode>> statements;

    for (auto& statement : m_statements)
        statements.emplace_back(statement ? statement->clone() : nullptr);

    return std::make_unique<BlockNode>(std::move(statements));
}
