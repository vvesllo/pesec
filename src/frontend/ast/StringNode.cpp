#include "include/frontend/ast/StringNode.hpp"


StringNode::StringNode(const std::string& value)
    : m_value(value)
{

}

Value StringNode::evaluate()
{
    return m_value;
}