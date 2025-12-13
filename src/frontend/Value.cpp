#include "include/frontend/Value.hpp"
#include "include/frontend/FunctionValue.hpp"

#include <stdexcept>
#include <sstream>


Value::Value()
    : m_type(ValueType::Null)
    , m_value(std::monostate{})
{}

Value::Value(double value)
    : m_type(ValueType::Double)
    , m_value(value)
{}

Value::Value(bool value)
    : m_type(ValueType::Boolean)
    , m_value(value)
{}

Value::Value(const std::string& value)
    : m_type(ValueType::String)
    , m_value(value)
{}

Value::Value(FunctionValue value)
    : m_type(ValueType::Function)
    , m_value(std::move(value))
{}

std::string Value::toString() const
{
    switch (m_type) 
    {
    case ValueType::Null: return "null";
    case ValueType::Boolean: return getBoolean() ? "true" : "false";
    case ValueType::String: return getString();
    case ValueType::Double:
    {
        std::ostringstream oss;
        oss.precision(std::numeric_limits<double>::max_digits10);
        oss << getDouble(); 
        return oss.str();
    }
    default: return "UNSTRINGABLE FUCK";
    }
}

bool Value::isDouble() const { return m_type == ValueType::Double; }

bool Value::isBoolean() const { return m_type == ValueType::Boolean; }

bool Value::isString() const { return m_type == ValueType::String; }

bool Value::isFunction() const { return m_type == ValueType::Function; }

double Value::getDouble() const
{
    if (isDouble()) 
        return std::get<double>(m_value);

    throw std::runtime_error("Value is not double");
}

bool Value::getBoolean() const
{    
    if (isBoolean()) 
        return std::get<bool>(m_value);

    throw std::runtime_error("Value is not boolean");
}

std::string Value::getString() const
{
    if (isString()) 
        return std::get<std::string>(m_value);

    throw std::runtime_error("Value is not string");
}

FunctionValue& Value::getFunction() const
{
    if (isFunction()) 
        return std::get<FunctionValue>(m_value);

    throw std::runtime_error("Value is not function");
}

Value Value::operator+(const Value& other) const
{
    if (isDouble() && other.isDouble())
        return Value(getDouble() + other.getDouble());
    else if (isString() && other.isString())
        return Value(getString() + other.getString());
    
    throw std::runtime_error("Values can't be added");
}

Value Value::operator-(const Value& other) const
{
    if (isDouble() && other.isDouble())
        return Value(getDouble() - other.getDouble());
   
    throw std::runtime_error("Values can't be substracted");
}

Value Value::operator*(const Value& other) const
{
    if (isDouble() && other.isDouble())
        return Value(getDouble() * other.getDouble());
    
    throw std::runtime_error("Values can't be multiplied");
}

Value Value::operator/(const Value& other) const
{
    if (other.isDouble() && other.getDouble() == 0)
        throw std::runtime_error("Division by zero");

    if (isDouble() && other.isDouble())
        return Value(getDouble() / other.getDouble());
   
    throw std::runtime_error("Values can't be divided");
}

Value Value::operator==(const Value& other) const
{
    if (isDouble() && other.isDouble())   return getDouble() == other.getDouble();
    if (isString() && other.isString())   return getString() == other.getString();
    if (isBoolean() && other.isBoolean()) return getBoolean() == other.getBoolean();

    throw std::runtime_error("Values can't be compared");
}

Value Value::operator!=(const Value& other) const
{
    if (isDouble() && other.isDouble())   return getDouble() != other.getDouble();
    if (isString() && other.isString())   return getString() != other.getString();
    if (isBoolean() && other.isBoolean()) return getBoolean() != other.getBoolean();

    throw std::runtime_error("Values can't be compared");
}

Value Value::operator<(const Value& other) const
{
    if (isDouble() && other.isDouble()) return getDouble() < other.getDouble();

    throw std::runtime_error("Values can't be compared");
}

Value Value::operator>(const Value& other) const
{
    if (isDouble() && other.isDouble()) return getDouble() > other.getDouble();

    throw std::runtime_error("Values can't be compared");
}

Value Value::operator<=(const Value& other) const
{
    if (isDouble() && other.isDouble()) return getDouble() <= other.getDouble();

    throw std::runtime_error("Values can't be compared");
}

Value Value::operator>=(const Value& other) const
{
    if (isDouble() && other.isDouble()) return getDouble() >= other.getDouble();

    throw std::runtime_error("Values can't be compared");
}
