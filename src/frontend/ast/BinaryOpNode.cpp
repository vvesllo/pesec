#include "include/frontend/ast/BinaryOpNode.hpp"

#include <stdexcept>


BinaryOpNode::BinaryOpNode(
    const std::string& op,
    std::unique_ptr<ASTNode> left,
    std::unique_ptr<ASTNode> right
)
    : m_op(op)
    , m_left(std::move(left))
    , m_right(std::move(right))
{

}

Value BinaryOpNode::evaluate()
{
    if      (m_op == "+") return m_left->evaluate() + m_right->evaluate();
    else if (m_op == "-") return m_left->evaluate() - m_right->evaluate();
    else if (m_op == "*") return m_left->evaluate() * m_right->evaluate();
    else if (m_op == "/") return m_left->evaluate() / m_right->evaluate();

    throw std::runtime_error("Unknown operation: " + m_op);
}