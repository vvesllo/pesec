#include "include/frontend/ast/IntegerNode.hpp"


IntegerNode::IntegerNode(long long value)
    : m_value(value)
{

}

Value IntegerNode::evaluate()
{
    return m_value;
}