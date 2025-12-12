#include "include/frontend/ast/BreakValueException.hpp"

BreakValueException::BreakValueException(Value value)
    : std::runtime_error("Break value exception")
    , m_value(value)
{

}

Value BreakValueException::value() const
{
    return m_value;
}