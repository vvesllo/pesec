#include "include/frontend/ast/Value.hpp"

#include <stdexcept>
#include <sstream>
#include <print>

Value::Value() {}

Value::Value(long long value)
    : m_type(ValueType::Integer)
    , m_value(value) {}

Value::Value(const std::string& value)
    : m_type(ValueType::String)
    , m_value(value) {}
    
bool Value::isInteger() const { return m_type == ValueType::Integer; }

bool Value::isString() const { return m_type == ValueType::String; }

long long Value::getInteger() const
{
    if (!isInteger())
        throw std::runtime_error("Value is not integer");
    return std::get<long long>(m_value);
}

std::string Value::getString() const
{
    if (!isString())
        throw std::runtime_error("Value is not string");

    return std::get<std::string>(m_value);
}

std::string Value::toString() const
{
    std::string result;


    switch (m_type)
    {
    case ValueType::Integer: result = std::to_string(getInteger()); break;
    case ValueType::String:  result = getString(); break;
    default: throw std::runtime_error("Value can't be string");
    }

    return result;
}

Value Value::operator+(const Value& other) const
{
    if (isInteger() && other.isInteger())
        return Value(getInteger() + other.getInteger());
    else if (isString() && other.isString())
        return Value(getString() + other.getString());

    throw std::runtime_error("Add values");
}

Value Value::operator-(const Value& other) const
{
    if (isInteger() && other.isInteger())
        return Value(getInteger() - other.getInteger());

    throw std::runtime_error("Subtraction values");
}

Value Value::operator*(const Value& other) const
{
    if (isInteger() && other.isInteger())
        return Value(getInteger() * other.getInteger());
    else if (isString() && other.isInteger())
    {
        std::stringstream result;
        
        for (long long i=0; i < other.getInteger(); i++)
            result << getString();

        return Value(result.str());
    }

    throw std::runtime_error("Multiply values");
}

Value Value::operator/(const Value& other) const
{
    if (isInteger() && other.isInteger())
    {
        if (other.isInteger() == 0)
            throw std::runtime_error("Divide by zero");

        return Value(getInteger() / other.getInteger());
    }

    throw std::runtime_error("Division values");
}
