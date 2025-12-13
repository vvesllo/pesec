#include "include/frontend/ast/ReturnValueException.hpp"

ReturnValueException::ReturnValueException(Value value)
    : std::runtime_error("Unexpected return usage")
    , m_value(value)
{

}

Value ReturnValueException::value() const
{
    return m_value;
}