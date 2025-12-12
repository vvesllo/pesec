#include "include/frontend/ast/ReturnValueException.hpp"

ReturnValueException::ReturnValueException(Value value)
    : std::runtime_error("Return value exception")
    , m_value(value)
{

}

Value ReturnValueException::value() const
{
    return m_value;
}