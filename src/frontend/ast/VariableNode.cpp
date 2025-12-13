#include "include/frontend/ast/VariableNode.hpp"


VariableNode::VariableNode(const std::string& name)
    : m_name(name)
{
    
}


Value VariableNode::evaluate(Context& context) const
{
    return context.get(m_name).value;
}

std::unique_ptr<ASTNode> VariableNode::clone() const
{
    return std::make_unique<VariableNode>(m_name);
} 