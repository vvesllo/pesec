#include "include/frontend/ast/BreakValueException.hpp"

BreakValueException::BreakValueException(Value value)
    : std::runtime_error("Unexpected break usage")
    , m_value(value)
{

}

Value BreakValueException::value() const
{
    return m_value;
}