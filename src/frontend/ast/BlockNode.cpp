#include "include/frontend/ast/BlockNode.hpp"



BlockNode::BlockNode(std::vector<std::unique_ptr<ASTNode>> statements)
    : m_statements(std::move(statements))
{

}

Value BlockNode::evaluate(Context& context) const
{
    Value value;
    
    for (auto& statement : m_statements)
        value = statement->evaluate(context);
    
    return value;
}
